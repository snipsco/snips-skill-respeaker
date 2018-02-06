from setuptools import setup

setup(
    name='snipsrespeaker',
    version='1.0',
    description='Respeaker skill for Snips',
    author='Snips Labs',
    author_email='labs@snips.ai',
    url='https://github.com/snipsco/snips-skill-respeaker',
    download_url='',
    license='MIT',
    install_requires=[
        'colour',
        'Adafruit_GPIO'
    ],
    test_suite="tests",
    keywords=['snips'],
    package_data={'snipsrespeaker': ['Snipsspec', 'config.json']},
    data_files=[('snipsrespeaker',['snipsrespeaker/config.json'])],
    packages=['snipsrespeaker', 'snipsrespeaker/APA102_Pi'],
    include_package_data=True
)
