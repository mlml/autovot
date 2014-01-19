/*
 * This software has been licensed to the Centre of Speech Technology, KTH
 * by Microsoft Corp. with the terms in the accompanying file BSD.txt,
 * which is a BSD style license.
 *
 *    "Copyright (c) 1990-1996 Entropic Research Laboratory, Inc. 
 *                   All rights reserved"
 *
 * Written by:  David Talkin
 * Checked by:
 * Revised by:  Derek Lin, David Talkin
 *
 * Brief description:
 *     A collection of pretty generic signal-processing routines.
 *
 *
 */

/*
 * sigproc.h
 * 
 */

int xget_window(float *dout, int n, int type);
void xrwindow(float *din, float *dout, int n, float preemp);
void xcwindow(float *din, float *dout, int n, float preemp);
void xhwindow(float *din, float *dout, int n, float preemp);
void xhnwindow(float *din, float *dout, int n, float preemp);
int window(float *din, float *dout, int n, float preemp, int type);
void xautoc(int windowsize, float *s, int p, float *r, float *e);
void xdurbin(float *r, float *k, float *a, int p, float *ex);
void xa_to_aca(float *a, float *b, float *c, int p);
float xitakura (int p, float *b, float *c, float *r, float *gain );
float wind_energy(float *data, int size, int w_type);
int xlpc(int lpc_ord, float lpc_stabl, int wsize, float *data, float *lpca, float *ar, float *lpck, float *normerr, float *rms, float preemp, int type);
void crossf(float *data, int size, int start, int nlags, float *engref, int *maxloc, float *maxval, float *correl);
void crossfi(float *data, int size, int start0, int nlags0, int nlags, float *engref, int *maxloc, float *maxval, float *correl, int *locs, int nlocs);
