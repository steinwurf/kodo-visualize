#! /usr/bin/env python
# encoding: utf-8

import os
import platform

APPNAME = 'kodo-visualize'
VERSION = '0.0.0'


def configure(conf):

    def locate_lib_manually(lib, option):
        path = getattr(conf.options, option)
        if not path:
            conf.fatal('Please specify path to the {lib} using the '
                       '--{option} option.'.format(lib=lib, option=option))

        path = os.path.abspath(os.path.expanduser(path))

        lib_dir = os.path.join(path, 'lib')

        if conf.is_mkspec_platform('windows'):
            if platform.machine().endswith('64'):
                target = 'x64'
            else:
                target = 'x86'

            if len(conf.env.MSVC_TARGETS) > 0:
                target = conf.env.MSVC_TARGETS[0]

            if 'amd64' in target:
                target = 'x64'

            lib_dir = os.path.join(lib_dir, target)

        conf.check_cxx(
            lib=lib,
            libpath=[lib_dir])

        conf.env['INCLUDES_{}'.format(lib)] = [os.path.join(path, 'include')]

    if conf.is_mkspec_platform('linux') or conf.is_mkspec_platform('mac'):
        conf.check_cfg(package='sdl2', args='--cflags --libs')
        conf.check_cfg(package='SDL2_image', args='--cflags --libs')
        conf.check_cfg(package='SDL2_ttf', args='--cflags --libs')
    else:
        locate_lib_manually('SDL2', 'sdl2_path')
        locate_lib_manually('SDL2_IMAGE', 'sdl2_image_path')
        locate_lib_manually('SDL2_TTF', 'sdl2_ttf_path')

    if conf.is_mkspec_platform('linux'):
        if not conf.env['LIB_PTHREAD']:
            # If we have not looked for pthread yet
            conf.check_cxx(lib='pthread')


def build(bld):

    bld.env.append_unique(
        'DEFINES_STEINWURF_VERSION',
        'STEINWURF_KODO_VISUALIZE_VERSION="{}"'.format(VERSION))

    bld.recurse('src/kodo_visualize')

    if bld.is_toplevel():

        # Only build tests when executed from the top-level wscript,
        # i.e. not when included as a dependency
        bld.recurse('test')

        bld.recurse('applications')

        bld.recurse('examples/recoding')
