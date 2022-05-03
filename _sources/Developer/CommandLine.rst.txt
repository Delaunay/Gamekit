Command Line
============

To increase productivity gamekit comes with command line interface
to setup, test & prepare projects for deployment.

Installation
------------

.. code-block::

   # Editable install
   pip install -e .

   # Install fron github
   pip install git+https://github.com/Delaunay/Gamekit.git


Usage
-----

* ``gkcli init``: initialize path to engine and project directories
* ``gkcli vscode <project>``: adds python stub to vscode paths
* ``gkcli tests <project> <map>``: runs all the tests for gamekit
* ``gkcli cook <project>``: Builds & cooks the project
