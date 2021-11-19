#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "plot.h"

#define LINE_SIZE                 4096

/* plot descriptor */
static struct plot_t plot;

/*
 * Close gnuplot.
 */
static void plot_close()
{
  if (plot.fp)
    pclose(plot.fp);

  unlink(plot.dataset);

  exit(0);
}

/*
 * Init gnuplot.
 */
void plot_init()
{
  /* open gnuplot */
  plot.fp = popen("gnuplot", "w");
  if (!plot.fp) {
    perror("popen");
    exit(1);
  }

  /* install signal handler */
  signal(SIGINT, plot_close);

  /* clear dataset */
  memset(plot.dataset, 0, DATASET_FILE_LEN);
}

/*
 * Plot a dataset.
 */
int plot_dataset(double *x, double *y, size_t nb_points)
{
  char line[LINE_SIZE];
  int ret, tmp_fd;
  size_t i, len;

  /* create temporary file */
  strncpy(plot.dataset, DATASET_FILE_PATTERN, DATASET_FILE_LEN);
  tmp_fd = mkstemp(plot.dataset);
  if (tmp_fd < 0) {
    ret = tmp_fd;
    perror("mkstemp");
    return ret;
  }

  /* write points */
  for (i = 0; i < nb_points; i++) {
    len = sprintf(line, "%f %f\n", x[i], y[i]);
    write(tmp_fd, line, len);
  }

  /* close temporary file */
  close(tmp_fd);

  /* print gnuplot command */
  fprintf(plot.fp, "plot \"%s\"\n", plot.dataset);
  fflush(plot.fp);

  /* infinite loop */
  for (;;);

  return ret;
}
