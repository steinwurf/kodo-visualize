#! /usr/bin/env python
# encoding: utf-8

import os
import platform

APPNAME = 'kodo-visualize'
VERSION = '0.0.0'


def recurse_helper(ctx, name):
    if not ctx.has_dependency_path(name):
        ctx.fatal('Load a tool to find %s as system dependency' % name)
    else:
        p = ctx.dependency_path(name)
        ctx.recurse([p])


def options(opt):

    import waflib.extras.wurf_dependency_bundle as bundle
    import waflib.extras.wurf_dependency_resolve as resolve

    bundle.add_dependency(opt, resolve.ResolveGitMajorVersion(
        name='boost',
        git_repository='github.com/steinwurf/boost.git',
        major_version=1))

    bundle.add_dependency(opt, resolve.ResolveGitMajorVersion(
        name='cpuid',
        git_repository='github.com/steinwurf/cpuid.git',
        major_version=3))

    bundle.add_dependency(opt, resolve.ResolveGitMajorVersion(
        name='fifi',
        git_repository='github.com/steinwurf/fifi.git',
        major_version=20))

    bundle.add_dependency(opt, resolve.ResolveGitMajorVersion(
        name='gtest',
        git_repository='github.com/steinwurf/gtest.git',
        major_version=2))

    bundle.add_dependency(opt, resolve.ResolveGitMajorVersion(
        name='kodo',
        git_repository='github.com/steinwurf/kodo.git',
        major_version=30))

    bundle.add_dependency(opt, resolve.ResolveGitMajorVersion(
        name='kodo-c',
        git_repository='github.com/steinwurf/kodo-c.git',
        major_version=6))

    bundle.add_dependency(opt, resolve.ResolveGitMajorVersion(
        name='kodo-cpp',
        git_repository='github.com/steinwurf/kodo-cpp.git',
        major_version=3))

    bundle.add_dependency(opt, resolve.ResolveGitMajorVersion(
        name='platform',
        git_repository='github.com/steinwurf/platform.git',
        major_version=1))

    bundle.add_dependency(opt, resolve.ResolveGitMajorVersion(
        name='recycle',
        git_repository='github.com/steinwurf/recycle.git',
        major_version=1))

    bundle.add_dependency(opt, resolve.ResolveGitMajorVersion(
        name='meta',
        git_repository='github.com/steinwurf/meta.git',
        major_version=1))

    bundle.add_dependency(opt, resolve.ResolveGitMajorVersion(
        name='sak',
        git_repository='github.com/steinwurf/sak.git',
        major_version=14))

    bundle.add_dependency(opt, resolve.ResolveGitMajorVersion(
        name='waf-tools',
        git_repository='github.com/steinwurf/waf-tools.git',
        major_version=2))

    opt.load("wurf_configure_output")
    opt.load('wurf_dependency_bundle')
    opt.load('wurf_tools')

    opt.add_option(
        '--sdl2_path',
        help='Path to SDL2 lib',
        action="store",
        type="string",
        default=None)

    opt.add_option(
        '--sdl2_image_path',
        help='Path to SDL2_image lib',
        action="store",
        type="string",
        default=None)

    opt.add_option(
        '--sdl2_ttf_path',
        help='Path to SDL2_tff lib',
        action="store",
        type="string",
        default=None)


def configure(conf):

    if conf.is_toplevel():

        conf.load('wurf_dependency_bundle')
        conf.load('wurf_tools')

        conf.load_external_tool('mkspec', 'wurf_cxx_mkspec_tool')
        conf.load_external_tool('runners', 'wurf_runner')
        conf.load_external_tool('install_path', 'wurf_install_path')
        conf.load_external_tool('project_gen', 'wurf_project_generator')

        recurse_helper(conf, 'boost')
        recurse_helper(conf, 'cpuid')
        recurse_helper(conf, 'platform')
        recurse_helper(conf, 'sak')
        recurse_helper(conf, 'recycle')
        recurse_helper(conf, 'meta')
        recurse_helper(conf, 'fifi')
        recurse_helper(conf, 'gtest')
        recurse_helper(conf, 'kodo')
        recurse_helper(conf, 'kodo-cpp')
        recurse_helper(conf, 'kodo-c')

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

    if bld.is_toplevel():

        bld.load('wurf_dependency_bundle')

        recurse_helper(bld, 'boost')
        recurse_helper(bld, 'cpuid')
        recurse_helper(bld, 'platform')
        recurse_helper(bld, 'sak')
        recurse_helper(bld, 'recycle')
        recurse_helper(bld, 'fifi')
        recurse_helper(bld, 'meta')
        recurse_helper(bld, 'gtest')
        recurse_helper(bld, 'kodo')
        recurse_helper(bld, 'kodo-c')
        recurse_helper(bld, 'kodo-cpp')

        bld.recurse('test')

        bld.recurse('applications')

        bld.recurse('examples/recoding')

    bld.recurse('src/kodo_visualize')

    bld.env.append_unique(
        'DEFINES_STEINWURF_VERSION',
        'STEINWURF_KODO_VISUALIZE_VERSION="{}"'.format(
            VERSION))
