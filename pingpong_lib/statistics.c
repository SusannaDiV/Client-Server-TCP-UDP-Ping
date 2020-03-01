#include <stdlib.h>
#include <stdio.h>
#include "pingpong.h"

double timespec_delta2milliseconds(struct timespec *last,
				   struct timespec *previous)
{
/*** This function computes the difference (last - previous)
 *** and returns the result expressed in milliseconds as a double
 ***/
	double diff_secInMillisec = (double)((last->tv_sec)-(previous->tv_sec))*1000;
	double diff_nsecInMillisec = (double)((last->tv_nsec)-(previous->tv_nsec))/1000000;
	return diff_secInMillisec+diff_nsecInMillisec;
}

int double_cmp(const void *p1, const void *p2)
{
	double d1 = *((const double *)p1);
	double d2 = *((const double *)p2);
	if (d1 > d2)
		return 1;
	if (d1 < d2)
		return -1;
	return 0;
}

void print_statistics(FILE * outf, const char *name, int repeats,
		      double rtt[repeats], int msg_sz, double resolution)
{
	const int N_HISTOGRAM_ITEMS = 21;
	int median = repeats / 2;
	int p10 = repeats / 10;
	int p90 = 9 * p10;
	int histogram[N_HISTOGRAM_ITEMS];
	double h_min, h_max, h_incr;
	int i, j;
	double mean, var, ns;

	qsort(rtt, (size_t)repeats, sizeof(double), double_cmp);
	printf("\n ... clock resolution was %lg", resolution);
	h_min = resolution * (double)((long)(rtt[0] / resolution)) - 0.5 * resolution;
	debug(" h_min=%lg, ", h_min);
	if (h_min < 0.0)
		h_min = 0.0;
	h_max = resolution * (double)((long)((rtt[repeats - 1] + 0.5 * resolution) / resolution)) + 0.5 * resolution;
	h_incr = (h_max - h_min) / (double)(N_HISTOGRAM_ITEMS - 1);
	debug(" h_max=%lg, h_incr=%lg\n", h_max, h_incr);
	for (j = 0; j < N_HISTOGRAM_ITEMS; j++)
		histogram[j] = 0;
	for (i = 0, mean = var = ns = 0.0; i < repeats; i++) {
		double v = rtt[i];
		double delta = v - mean;
		debug("     rtt[%d]=%lg\n", i, rtt[i]);
		ns += 1.0;
		mean += delta / ns;
		if (ns > 1.5) {
			var += delta * (v - mean);
		}
		v -= h_min;
		v /= h_incr;
		v += 0.5;
		j = (int)v;
		histogram[j]++;
	}
	var /= (double)(repeats - 1);
	fprintf(outf, "\n%s Statistics over %d repetitions of %d byte messages\n\n", name, repeats, msg_sz);
	fprintf(outf, "RTT : percentile 10: %lg, median: %lg, percentile 90: %lg, average: %lg, variance: %lg\n\n", rtt[p10], rtt[median], rtt[p90], mean, var);
	fprintf(outf, "RTT histogram:\n");

	for (j = 0; j < N_HISTOGRAM_ITEMS; ++j) {
		fprintf(outf, "%f %d\n", h_min+=h_incr, histogram[j]);
	}
	fprintf(outf, "\n   median Throughput : %lg KB/s", 2.0 * (double)(msg_sz) / rtt[median]);

	if (mean > 0.0)
		fprintf(outf, "   overall Throughput : %lg KB/s", 2.0 * (double)(msg_sz) / mean);
	fprintf(outf, "\n\n");
}

