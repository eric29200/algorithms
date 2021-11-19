#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "plot.h"
#include "../utils/mem.h"

#define LINE_SIZE                 4096

/* plot descriptor */
static struct plot_t plot;

/*
 * Close gnuplot.
 */
void plot_close()
{
  size_t i;

  /* close gnuplot */
  if (plot.fp)
    pclose(plot.fp);

  /* delete datasets */
  if (plot.datasets) {
    for (i = 0; i < plot.nb_datasets; i++) {
      unlink(plot.datasets[i]);
      free(plot.datasets[i]);
    }

    free(plot.datasets);
  }

  /* exit */
  exit(0);
}

/*
 * Init gnuplot.
 */
void plot_init()
{
  /* clear datasets */
  plot.datasets = NULL;
  plot.nb_datasets = 0;

  /* open gnuplot */
  plot.fp = popen("gnuplot", "w");
  if (!plot.fp) {
    perror("popen");
    exit(1);
  }

  /* install signal handler */
  signal(SIGINT, plot_close);
}

/*
 * Plot a dataset.
 */
int plot_add_dataset(struct point_t *points, size_t nb_points)
{
  char line[LINE_SIZE];
  int ret, tmp_fd;
  size_t i, len;

  /* add a dataset */
  plot.datasets = (char **) xrealloc(plot.datasets, sizeof(char *) * (plot.nb_datasets + 1));
  plot.datasets[plot.nb_datasets] = (char *) xmalloc(DATASET_FILE_LEN);
  plot.nb_datasets++;

  /* create temporary file */
  strncpy(plot.datasets[plot.nb_datasets - 1], DATASET_FILE_PATTERN, DATASET_FILE_LEN);
  tmp_fd = mkstemp(plot.datasets[plot.nb_datasets - 1]);
  if (tmp_fd < 0) {
    ret = tmp_fd;
    perror("mkstemp");
    return ret;
  }

  /* write points */
  for (i = 0; i < nb_points; i++) {
    len = sprintf(line, "%f %f\n", points[i].x, points[i].y);
    write(tmp_fd, line, len);
  }

  /* close temporary file */
  close(tmp_fd);

  return 0;
}

/*
 * Print gnuplot.
 */
void plot_print()
{
  char cmd[LINE_SIZE];
  size_t i;

  /* no datasets */
  if (plot.nb_datasets <= 0)
    return;

  /* create cmd line */
  sprintf(cmd, "plot \"%s\"", plot.datasets[0]);
  for (i = 1; i < plot.nb_datasets; i++) {
    strcat(cmd, ",\"");
    strcat(cmd, plot.datasets[i]);
    strcat(cmd, "\"");
  }
  strcat(cmd, "\n");

  /* print gnuplot command */
  fputs(cmd, plot.fp);
  fflush(plot.fp);

  /* infinite loop */
  for (;;);
}
