from distutils.core import setup

VERSION = '1.0'

setup(name='chopper_control',
    version=VERSION,
    description='Controls the motor for the chopper box.',
    author='Carlos Sierra',
    author_email='csierra@uchicago.edu',
    package_dir={'chopper_agent':'chopper_client'},
    packages = ['chopper_agent'])
