# Makefile for program source directory in GNU NLS utilities package.
# Copyright (C) 1995, 1996, 1997 by Ulrich Drepper <drepper@gnu.ai.mit.edu>
#
# This file file be copied and used freely without restrictions.  It can
# be used in projects which are not available under the GNU Public License
# but which still want to provide support for the GNU gettext functionality.
# Please note that the actual code is *not* freely available.
#
# - Modified by Owen Taylor <otaylor@redhat.com> to use GETTEXT_PACKAGE
#   instead of PACKAGE and to look for po2tbl in ./ not in intl/
#
# - Modified by jacob berkman <jacob@ximian.com> to install
#   Makefile.in.in and po2tbl.sed.in for use with glib-gettextize

GETTEXT_PACKAGE = vips7
PACKAGE = vips
VERSION = 7.22.1

SHELL = /bin/bash


srcdir = /home/dsuo/turkey/apps/vips/src/po
top_srcdir = /home/dsuo/turkey/apps/vips/src
VPATH = /home/dsuo/turkey/apps/vips/src/po

prefix = /usr/local
exec_prefix = ${prefix}
datarootdir = ${prefix}/share
datadir = ${datarootdir}
libdir = ${exec_prefix}/lib
localedir = $(libdir)/locale
gnulocaledir = $(datadir)/locale
gettextsrcdir = $(datadir)/glib-2.0/gettext/po
subdir = po

INSTALL = /usr/bin/install -c
INSTALL_DATA = ${INSTALL} -m 644
MKINSTALLDIRS = $(top_srcdir)//home/dsuo/turkey/apps/vips/src/mkinstalldirs

CC = gcc
GENCAT = @GENCAT@
GMSGFMT = /usr/bin/msgfmt
MSGFMT = /usr/bin/msgfmt
MSGFMT_OPTS = -c
XGETTEXT = /usr/bin/xgettext
MSGMERGE = msgmerge

DEFS = -DHAVE_CONFIG_H
CFLAGS = -g -O2
CPPFLAGS = 

INCLUDES = -I.. -I$(top_srcdir)/intl

COMPILE = $(CC) -c $(DEFS) $(INCLUDES) $(CPPFLAGS) $(CFLAGS) $(XCFLAGS)

SOURCES = 
POFILES =  en_GB.po de.po
GMOFILES =  en_GB.gmo de.gmo
DISTFILES = LINGUAS ChangeLog Makefile.in.in POTFILES.in $(GETTEXT_PACKAGE).pot \
$(POFILES) $(GMOFILES) $(SOURCES)

POTFILES = \
	/home/dsuo/turkey/apps/vips/src/libvips/acquire/im_clamp.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_deviate.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_point_bilinear.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_linreg.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/arith_dispatch.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_subtract.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_stats.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_maxpos.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_maxpos_avg.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_maxpos_vec.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_invert.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_sign.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/round.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_measure.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/math.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_minpos.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_abs.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_recomb.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_cross_phase.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_divide.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_bandmean.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_lintra.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_remainder.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/power.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_add.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_avg.c \
	/home/dsuo/turkey/apps/vips/src/libvips/arithmetic/im_multiply.c \
	/home/dsuo/turkey/apps/vips/src/libvips/boolean/boolean.c \
	/home/dsuo/turkey/apps/vips/src/libvips/boolean/bool_dispatch.c \
	/home/dsuo/turkey/apps/vips/src/libvips/cimg/cimg_dispatch.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_rad2float.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_LCh2UCS.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_icc_transform.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/disp.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_dE_fromLab.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_Yxy2XYZ.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_LabQ2disp.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_Lab2LabS.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_LabQ2Lab.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_UCS2LCh.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_LabS2Lab.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_dECMC_fromLab.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_LabQ2LabS.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_Lab2XYZ.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_disp2XYZ.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_XYZ2disp.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/colour.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_float2rad.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_Lab2LCh.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/colour_dispatch.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/derived.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_LabS2LabQ.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_dE00_fromLab.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_lab_morph.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_XYZ2Yxy.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_Lab2LabQ.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_LCh2Lab.c \
	/home/dsuo/turkey/apps/vips/src/libvips/colour/im_XYZ2Lab.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_insert.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_mask2vips.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_gaussnoise.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_flipver.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_falsecolour.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_vips2mask.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_clip2fmt.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_ri2c.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_system.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_text.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_zoom.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/conver_dispatch.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_copy_file.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_c2real.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_grid.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_c2imag.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_scaleps.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_fliphor.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_scale.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_black.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_tbjoin.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_replicate.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_embed.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_lrjoin.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_rot180.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_extract.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_wrap.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_c2rect.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_copy.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_rot90.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_c2amph.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_msb.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_system_image.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_subsample.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_rot270.c \
	/home/dsuo/turkey/apps/vips/src/libvips/conversion/im_gbandjoin.c \
	/home/dsuo/turkey/apps/vips/src/libvips/convolution/im_contrast_surface.c \
	/home/dsuo/turkey/apps/vips/src/libvips/convolution/im_gradcor.c \
	/home/dsuo/turkey/apps/vips/src/libvips/convolution/im_compass.c \
	/home/dsuo/turkey/apps/vips/src/libvips/convolution/im_convsep.c \
	/home/dsuo/turkey/apps/vips/src/libvips/convolution/im_conv.c \
	/home/dsuo/turkey/apps/vips/src/libvips/convolution/im_fastcor.c \
	/home/dsuo/turkey/apps/vips/src/libvips/convolution/convol_dispatch.c \
	/home/dsuo/turkey/apps/vips/src/libvips/convolution/im_conv_f.c \
	/home/dsuo/turkey/apps/vips/src/libvips/convolution/im_addgnoise.c \
	/home/dsuo/turkey/apps/vips/src/libvips/convolution/im_spcor.c \
	/home/dsuo/turkey/apps/vips/src/libvips/convolution/im_convsep_f.c \
	/home/dsuo/turkey/apps/vips/src/libvips/convolution/im_sharpen.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/im_tile_cache.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/im_ppm2vips.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/format.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/im_vips2tiff.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/im_vips2csv.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/im_vips2ppm.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/im_png2vips.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/format_dispatch.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/im_analyze2vips.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/im_magick2vips.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/im_vips2raw.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/im_tiff2vips.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/im_exr2vips.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/matlab.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/im_jpeg2vips.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/im_csv2vips.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/im_vips2png.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/radiance.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/im_raw2vips.c \
	/home/dsuo/turkey/apps/vips/src/libvips/format/im_vips2jpeg.c \
	/home/dsuo/turkey/apps/vips/src/libvips/freq_filt/im_freq_mask.c \
	/home/dsuo/turkey/apps/vips/src/libvips/freq_filt/im_phasecor_fft.c \
	/home/dsuo/turkey/apps/vips/src/libvips/freq_filt/fft_sp.c \
	/home/dsuo/turkey/apps/vips/src/libvips/freq_filt/fmaskcir.c \
	/home/dsuo/turkey/apps/vips/src/libvips/freq_filt/im_fractsurf.c \
	/home/dsuo/turkey/apps/vips/src/libvips/freq_filt/im_freqflt.c \
	/home/dsuo/turkey/apps/vips/src/libvips/freq_filt/fmask4th.c \
	/home/dsuo/turkey/apps/vips/src/libvips/freq_filt/im_invfft.c \
	/home/dsuo/turkey/apps/vips/src/libvips/freq_filt/freq_dispatch.c \
	/home/dsuo/turkey/apps/vips/src/libvips/freq_filt/im_rotquad.c \
	/home/dsuo/turkey/apps/vips/src/libvips/freq_filt/im_invfftr.c \
	/home/dsuo/turkey/apps/vips/src/libvips/freq_filt/im_fwfft.c \
	/home/dsuo/turkey/apps/vips/src/libvips/freq_filt/im_disp_ps.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_gammacorrect.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_hist.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_project.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_stdif.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_mpercent.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_histnD.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_histspec.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_identity.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_histindexed.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/hist_dispatch.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_hsp.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_histgr.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_buildlut.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/tone.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_histeq.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_histplot.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_lhisteq.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_invertlut.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_heq.c \
	/home/dsuo/turkey/apps/vips/src/libvips/histograms_lut/im_maplut.c \
	/home/dsuo/turkey/apps/vips/src/libvips/inplace/smudge_area.c \
	/home/dsuo/turkey/apps/vips/src/libvips/inplace/flood.c \
	/home/dsuo/turkey/apps/vips/src/libvips/inplace/im_insertplace.c \
	/home/dsuo/turkey/apps/vips/src/libvips/inplace/plot_point.c \
	/home/dsuo/turkey/apps/vips/src/libvips/inplace/line_draw.c \
	/home/dsuo/turkey/apps/vips/src/libvips/inplace/im_plotmask.c \
	/home/dsuo/turkey/apps/vips/src/libvips/inplace/im_paintrect.c \
	/home/dsuo/turkey/apps/vips/src/libvips/inplace/inplace_dispatch.c \
	/home/dsuo/turkey/apps/vips/src/libvips/inplace/im_circle.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/check.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/sink.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/rect.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/semaphore.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/memory.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/region.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_setbuf.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/callback.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_wrapmany.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_guess_prefix.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_generate.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/init.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/sinkscreen.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_mapfile.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/package.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/debug.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_init_world.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_partial.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/meta.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_setupout.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/base64.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/time.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_histlin.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/object.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_image.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_cp_desc.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/buf.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/util.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_prepare.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_binfile.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/header.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/sinkmemory.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/window.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/sinkdisc.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_demand_hint.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/buffer.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_open.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/error.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/dispatch_types.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/threadpool.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_writeline.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_open_vips.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_unmapfile.c \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/im_close.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mask/im_matcat.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mask/rw_mask.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mask/im_matinv.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mask/im_mattrn.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mask/im_matmul.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mask/im_logmasks.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mask/im_gaussmasks.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mask/rotmask.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mask/matalloc.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mask/mask_dispatch.c \
	/home/dsuo/turkey/apps/vips/src/libvips/morphology/im_profile.c \
	/home/dsuo/turkey/apps/vips/src/libvips/morphology/im_label_regions.c \
	/home/dsuo/turkey/apps/vips/src/libvips/morphology/morph_dispatch.c \
	/home/dsuo/turkey/apps/vips/src/libvips/morphology/im_zerox.c \
	/home/dsuo/turkey/apps/vips/src/libvips/morphology/im_erode.c \
	/home/dsuo/turkey/apps/vips/src/libvips/morphology/im_cntlines.c \
	/home/dsuo/turkey/apps/vips/src/libvips/morphology/im_rank.c \
	/home/dsuo/turkey/apps/vips/src/libvips/morphology/im_dilate.c \
	/home/dsuo/turkey/apps/vips/src/libvips/morphology/im_rank_image.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/mosaicing_dispatch.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/im_lrmerge.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/im_remosaic.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/im_tbmosaic.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/im_chkpair.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/im_tbcalcon.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/global_balance.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/im_tbmerge.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/im_maxpos_subpel.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/im_improve.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/im_avgdxdy.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/im_align_bands.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/im_lrcalcon.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/im_initialize.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/match.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/mosaic1.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/im_clinear.c \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/im_lrmosaic.c \
	/home/dsuo/turkey/apps/vips/src/libvips/other/im_simcontr.c \
	/home/dsuo/turkey/apps/vips/src/libvips/other/glds_funcs.c \
	/home/dsuo/turkey/apps/vips/src/libvips/other/cooc_funcs.c \
	/home/dsuo/turkey/apps/vips/src/libvips/other/im_grey.c \
	/home/dsuo/turkey/apps/vips/src/libvips/other/other_dispatch.c \
	/home/dsuo/turkey/apps/vips/src/libvips/other/im_benchmark.c \
	/home/dsuo/turkey/apps/vips/src/libvips/other/im_meanstd.c \
	/home/dsuo/turkey/apps/vips/src/libvips/other/im_zone.c \
	/home/dsuo/turkey/apps/vips/src/libvips/other/im_sines.c \
	/home/dsuo/turkey/apps/vips/src/libvips/other/im_eye.c \
	/home/dsuo/turkey/apps/vips/src/libvips/other/im_spatres.c \
	/home/dsuo/turkey/apps/vips/src/libvips/other/im_make_xy.c \
	/home/dsuo/turkey/apps/vips/src/libvips/other/im_dif_std.c \
	/home/dsuo/turkey/apps/vips/src/libvips/relational/im_ifthenelse.c \
	/home/dsuo/turkey/apps/vips/src/libvips/relational/relational_dispatch.c \
	/home/dsuo/turkey/apps/vips/src/libvips/relational/relational.c \
	/home/dsuo/turkey/apps/vips/src/libvips/relational/im_blend.c \
	/home/dsuo/turkey/apps/vips/src/libvips/resample/im_affine.c \
	/home/dsuo/turkey/apps/vips/src/libvips/resample/im_stretch3.c \
	/home/dsuo/turkey/apps/vips/src/libvips/resample/transform.c \
	/home/dsuo/turkey/apps/vips/src/libvips/resample/im_shrink.c \
	/home/dsuo/turkey/apps/vips/src/libvips/resample/resample_dispatch.c \
	/home/dsuo/turkey/apps/vips/src/libvips/resample/interpolate.c \
	/home/dsuo/turkey/apps/vips/src/libvips/resample/im_rightshift_size.c \
	/home/dsuo/turkey/apps/vips/src/libvips/video/video_dispatch.c \
	/home/dsuo/turkey/apps/vips/src/libvips/video/im_video_test.c \
	/home/dsuo/turkey/apps/vips/src/libvips/video/im_video_v4l1.c \
	/home/dsuo/turkey/apps/vips/src/tools/iofuncs/vips.c \
	/home/dsuo/turkey/apps/vips/src/tools/iofuncs/edvips.c \
	/home/dsuo/turkey/apps/vips/src/tools/iofuncs/vipsthumbnail.c \
	/home/dsuo/turkey/apps/vips/src/tools/iofuncs/header.c \
	/home/dsuo/turkey/apps/vips/src/tools/mosaicing/find_mosaic.c \
	/home/dsuo/turkey/apps/vips/src/tools/mosaicing/mergeup.c \
	/home/dsuo/turkey/apps/vips/src/tools/other/simcontr.c \
	/home/dsuo/turkey/apps/vips/src/tools/other/squares.c \
	/home/dsuo/turkey/apps/vips/src/tools/other/sines.c \
	/home/dsuo/turkey/apps/vips/src/tools/other/glds_features.c \
	/home/dsuo/turkey/apps/vips/src/tools/other/glds.c \
	/home/dsuo/turkey/apps/vips/src/tools/other/spatres.c \
	/home/dsuo/turkey/apps/vips/src/tools/other/cooc.c \
	/home/dsuo/turkey/apps/vips/src/tools/other/cooc_features.c \
	/home/dsuo/turkey/apps/vips/src/libvipsCC/VDisplay.cc \
	/home/dsuo/turkey/apps/vips/src/libvipsCC/VError.cc \
	/home/dsuo/turkey/apps/vips/src/libvipsCC/VImage.cc \
	/home/dsuo/turkey/apps/vips/src/libvipsCC/vipsc++.cc \
	/home/dsuo/turkey/apps/vips/src/libvipsCC/VMask.cc \
	/home/dsuo/turkey/apps/vips/src/libvipsCC/include/vips/VDisplay.h \
	/home/dsuo/turkey/apps/vips/src/libvipsCC/include/vips/VImage.h \
	/home/dsuo/turkey/apps/vips/src/libvipsCC/include/vips/vipscpp.h \
	/home/dsuo/turkey/apps/vips/src/libvipsCC/include/vips/VError.h \
	/home/dsuo/turkey/apps/vips/src/libvipsCC/include/vips/VMask.h \
	/home/dsuo/turkey/apps/vips/src/libvipsCC/include/vips/vipsc++.h \
	/home/dsuo/turkey/apps/vips/src/libvips/cimg/CImg.h \
	/home/dsuo/turkey/apps/vips/src/libvips/format/dbh.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/type.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/struct.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/deprecated.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/image.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/mosaicing.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/freq_filt.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/buf.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/threadpool.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/header.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/inlines.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/generate.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/cimg_funcs.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/object.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/other.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/util.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/inplace.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/version.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/private.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/boolean.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/conversion.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/almostdeprecated.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/resample.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/dispatch.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/rect.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/semaphore.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/disp.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/colour.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/intl.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/region.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/morphology.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/format.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/internal.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/interpolate.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/check.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/memory.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/meta.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/transform.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/error.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/convolution.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/callback.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/mask.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/vips.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/debug.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/histograms_lut.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/thread.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/relational.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/video.h \
	/home/dsuo/turkey/apps/vips/src/libvips/include/vips/arithmetic.h \
	/home/dsuo/turkey/apps/vips/src/libvips/iofuncs/base64.h \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/mosaic.h \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/merge.h \
	/home/dsuo/turkey/apps/vips/src/libvips/mosaicing/global_balance.h \
	/home/dsuo/turkey/apps/vips/src/libvips/resample/templates.h \
	/home/dsuo/turkey/apps/vips/src/libvips/video/im_video_v4l1.h \
	/home/dsuo/turkey/apps/vips/src/libvips/cimg/cimg.cpp \
	/home/dsuo/turkey/apps/vips/src/libvips/resample/bicubic.cpp \
	/home/dsuo/turkey/apps/vips/src/libvips/resample/nohalo.cpp \
	/home/dsuo/turkey/apps/vips/src/libvips/resample/vsqbs.cpp \
	/home/dsuo/turkey/apps/vips/src/libvips/resample/lbb.cpp

CATALOGS =  en_GB.gmo de.gmo
CATOBJEXT = .gmo
INSTOBJEXT = .mo

.SUFFIXES:
.SUFFIXES: .c .o .po .pox .gmo .mo .msg .cat

.c.o:
	$(COMPILE) $<

.po.pox:
	$(MAKE) $(GETTEXT_PACKAGE).pot
	$(MSGMERGE) $< $(srcdir)/$(GETTEXT_PACKAGE).pot -o $*.pox

.po.mo:
	$(MSGFMT) -o $@ $<

.po.gmo:
	file=$(srcdir)/`echo $* | sed 's,.*/,,'`.gmo \
	  && rm -f $$file && $(GMSGFMT) $(MSGFMT_OPTS) -o $$file $<

.po.cat:
	sed -f ../intl/po2msg.sed < $< > $*.msg \
	  && rm -f $@ && $(GENCAT) $@ $*.msg


all: all-yes

all-yes: $(CATALOGS)
all-no:

$(srcdir)/$(GETTEXT_PACKAGE).pot: $(POTFILES)
	$(XGETTEXT) --default-domain=$(GETTEXT_PACKAGE) --directory=$(top_srcdir) \
	  --add-comments --keyword=_ --keyword=N_ \
	  --keyword=C_:1c,2 \
	  --keyword=NC_:1c,2 \
	  --keyword=g_dngettext:2,3 \
	  --flag=N_:1:pass-c-format \
	  --flag=C_:2:pass-c-format \
	  --flag=NC_:2:pass-c-format \
	  --flag=g_dngettext:2:pass-c-format \
          --flag=g_strdup_printf:1:c-format \
          --flag=g_string_printf:2:c-format \
          --flag=g_string_append_printf:2:c-format \
          --flag=g_error_new:3:c-format \
          --flag=g_set_error:4:c-format \
          --flag=g_markup_printf_escaped:1:c-format \
          --flag=g_log:3:c-format \
          --flag=g_print:1:c-format \
          --flag=g_printerr:1:c-format \
          --flag=g_printf:1:c-format \
          --flag=g_fprintf:2:c-format \
          --flag=g_sprintf:2:c-format \
          --flag=g_snprintf:3:c-format \
          --flag=g_scanner_error:2:c-format \
          --flag=g_scanner_warn:2:c-format \
	  --files-from=$(srcdir)/POTFILES.in \
	&& test ! -f $(GETTEXT_PACKAGE).po \
	   || ( rm -f $(srcdir)/$(GETTEXT_PACKAGE).pot \
		&& mv $(GETTEXT_PACKAGE).po $(srcdir)/$(GETTEXT_PACKAGE).pot )

install: install-exec install-data
install-exec:
install-data: install-data-yes
install-data-no: all
install-data-yes: all
	if test -r "$(MKINSTALLDIRS)"; then \
	  $(MKINSTALLDIRS) $(DESTDIR)$(datadir); \
	else \
	  $(SHELL) $(top_srcdir)/mkinstalldirs $(DESTDIR)$(datadir); \
	fi
	@catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  case "$$cat" in \
	    *.gmo) destdir=$(gnulocaledir);; \
	    *)     destdir=$(localedir);; \
	  esac; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  dir=$(DESTDIR)$$destdir/$$lang/LC_MESSAGES; \
	  if test -r "$(MKINSTALLDIRS)"; then \
	    $(MKINSTALLDIRS) $$dir; \
	  else \
	    $(SHELL) $(top_srcdir)/mkinstalldirs $$dir; \
	  fi; \
	  if test -r $$cat; then \
	    $(INSTALL_DATA) $$cat $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	    echo "installing $$cat as $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT)"; \
	  else \
	    $(INSTALL_DATA) $(srcdir)/$$cat $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	    echo "installing $(srcdir)/$$cat as" \
		 "$$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT)"; \
	  fi; \
	  if test -r $$cat.m; then \
	    $(INSTALL_DATA) $$cat.m $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	    echo "installing $$cat.m as $$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m"; \
	  else \
	    if test -r $(srcdir)/$$cat.m ; then \
	      $(INSTALL_DATA) $(srcdir)/$$cat.m \
		$$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	      echo "installing $(srcdir)/$$cat as" \
		   "$$dir/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m"; \
	    else \
	      true; \
	    fi; \
	  fi; \
	done
	if test "$(PACKAGE)" = "glib"; then \
	  if test -r "$(MKINSTALLDIRS)"; then \
	    $(MKINSTALLDIRS) $(DESTDIR)$(gettextsrcdir); \
	  else \
	    $(SHELL) $(top_srcdir)/mkinstalldirs $(DESTDIR)$(gettextsrcdir); \
	  fi; \
	  $(INSTALL_DATA) $(srcdir)/Makefile.in.in \
			  $(DESTDIR)$(gettextsrcdir)/Makefile.in.in; \
	else \
	  : ; \
	fi

# Define this as empty until I found a useful application.
installcheck:

uninstall:
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	  rm -f $(DESTDIR)$(localedir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	  rm -f $(DESTDIR)$(gnulocaledir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT); \
	  rm -f $(DESTDIR)$(gnulocaledir)/$$lang/LC_MESSAGES/$(GETTEXT_PACKAGE)$(INSTOBJEXT).m; \
	done
	if test "$(PACKAGE)" = "glib"; then \
	  rm -f $(DESTDIR)$(gettextsrcdir)/Makefile.in.in; \
	fi

check: all

dvi info tags TAGS ID:

mostlyclean:
	rm -f core core.* *.pox $(GETTEXT_PACKAGE).po *.old.po cat-id-tbl.tmp
	rm -fr *.o

clean: mostlyclean

distclean: clean
	rm -f Makefile Makefile.in POTFILES *.mo *.msg *.cat *.cat.m

maintainer-clean: distclean
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."
	rm -f $(GMOFILES)

distdir = ../$(GETTEXT_PACKAGE)-$(VERSION)/$(subdir)
dist distdir: update-po $(DISTFILES)
	dists="$(DISTFILES)"; \
	for file in $$dists; do \
	  ln $(srcdir)/$$file $(distdir) 2> /dev/null \
	    || cp -p $(srcdir)/$$file $(distdir); \
	done

update-po: Makefile
	$(MAKE) $(GETTEXT_PACKAGE).pot
	tmpdir=`pwd`; \
	cd $(srcdir); \
	catalogs='$(CATALOGS)'; \
	for cat in $$catalogs; do \
	  cat=`basename $$cat`; \
	  lang=`echo $$cat | sed 's/\$(CATOBJEXT)$$//'`; \
	  echo "$$lang:"; \
	  if $(MSGMERGE) $$lang.po $(GETTEXT_PACKAGE).pot -o $$tmpdir/$$lang.new.po; then \
	    if cmp $$lang.po $$tmpdir/$$lang.new.po >/dev/null 2>&1; then \
	      rm -f $$tmpdir/$$lang.new.po; \
            else \
	      if mv -f $$tmpdir/$$lang.new.po $$lang.po; then \
	        :; \
	      else \
	        echo "msgmerge for $$lang.po failed: cannot move $$tmpdir/$$lang.new.po to $$lang.po" 1>&2; \
	        rm -f $$tmpdir/$$lang.new.po; \
	        exit 1; \
	      fi; \
	    fi; \
	  else \
	    echo "msgmerge for $$cat failed!"; \
	    rm -f $$tmpdir/$$lang.new.po; \
	  fi; \
	done

# POTFILES is created from POTFILES.in by stripping comments, empty lines
# and Intltool tags (enclosed in square brackets), and appending a full
# relative path to them
POTFILES: POTFILES.in
	( if test 'x$(srcdir)' != 'x.'; then \
	    posrcprefix='$(top_srcdir)/'; \
	  else \
	    posrcprefix="../"; \
	  fi; \
	  rm -f $@-t $@ \
	    && (sed -e '/^#/d' 						\
		    -e "s/^\[.*\] +//" 					\
		    -e '/^[ 	]*$$/d' 				\
		    -e "s@.*@	$$posrcprefix& \\\\@" < $(srcdir)/$@.in	\
		| sed -e '$$s/\\$$//') > $@-t \
	    && chmod a-w $@-t \
	    && mv $@-t $@ )

Makefile: Makefile.in.in ../config.status POTFILES
	cd .. \
	  && $(SHELL) ./config.status $(subdir)/$@.in

# Tell versions [3.59,3.63) of GNU make not to export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
