import os
from distutils.core import setup, Extension

path = os.path.join(os.path.dirname(__file__), "src")

module1 = Extension('testpkg',
                    define_macros=[('MAJOR_VERSION', '1'),
                                   ('MINOR_VERSION', '1')],
                    # include_dirs = ['/usr/local/include'],
                    # libraries = ['tcl83'],
                    # library_dirs = ['/usr/local/lib'],
                    sources=os.listdir(path))

setup(name='TestPkg',
      version='1.1',
      description='This is a demo package',
      author='Atabey Onur',
      author_email='a.onur@munichelectrification.com',
      url='https://github.com/1atabey1',
      long_description='''
This is really just a demo package.
''',
      ext_modules=[module1])
