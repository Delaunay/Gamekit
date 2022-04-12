from setuptools import setup


setup_kwargs = dict(
    name='gamekit',
    version='0.0.0',
    description='Gamekit python utilities',
    author='Pierre Delaunay',
    author_email='pierre@delaunay.io',
    license='BSD-3-Clause',
    url="https://gamekit.readthedocs.io",
    packages=[
        'gamekit',
        'gamekit.commands',
    ],
    package_dir={"": "Source/Python"},
    zip_safe=True,
    setup_requires=[],
    install_requires=[],
    python_requires='>=3.7.*',
    entry_points={
            'console_scripts': [
                'gkcli = gamekit.cli:main',
            ]
        }
)

if __name__ == "__main__":
    setup(**setup_kwargs)
