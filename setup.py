from setuptools import setup

setup(
    name='sniprespeaker',
    version='1.0',
    description='Respeaker skill for Snips',
    author='Snips Labs',
    author_email='labs@snips.ai',
    url='https://github.com/snipsco/snips-skill-respeaker',
    download_url='',
    license='MIT',
    install_requires=[
        'colour'
    ],
    test_suite="tests",
    keywords=['snips'],
    package_data={'sniprespeaker': ['Snipsspec']},
    packages=['snipsrespeaker'],
    include_package_data=True
)
