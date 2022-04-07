from setuptools import setup



setup_kwargs = dict(
    name='gamkekit',
    version='0.0.0',
    description='Gamekit python utilities',
    author='Pierre Delaunay',
    author_email='pierre@delaunay.io',
    license='BSD-3-Clause',
    url="https://gamekit.readthedocs.io",
    packages='gamekit',
    package_dir={"": "Source/python"},
    zip_safe=True,
    setup_requires=[],
    install_requires=[],
    python_requires='>=3.6.*',
)

if __name__ == "__main__":
    setup(**setup_kwargs)
