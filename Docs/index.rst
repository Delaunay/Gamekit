Welcome to GameKit's documentation!
===================================

.. image:: https://readthedocs.org/projects/gamekit/badge/?version=latest
   :target: https://gamekit.readthedocs.io/en/latest/?badge=latest
   :alt: Documentation Status


.. image:: https://github.com/Delaunay/Gamekit/blob/master/Docs/_static/Patron.png?raw=true
   :target: https://www.patreon.com/bePatron?u=10394303
   :alt: Patreon

Goals
~~~~~

High level game framework built on top of Unreal Engine.

* Data Driven
   * Configure your ability in json
   * Bootstrap Game wikis with the raw data

* Multiplayer Ready
   * Hosted
   * Dedicated
   * Standalone

* Ever growing documentation
   * Architecture Diagram
   * Replication Diagram
   * Code Search

* Community Friendly
   * Favour source code (C++, HLSL) for easier contribution workflow

* Base UI & Menus for debugging and prototyping

* Automation framework on top of UAT
   * CI/CD
   * Testing Utilities


.. warning::

   This is not meant to be an instroduction to UE.
   It expects some knowledge of UE and does not shy away from C++.

.. toctree::
   :caption: Welcome
   :maxdepth: 1

   Welcome/Installation

.. toctree::
   :caption: Data Driven
   :maxdepth: 1

   DataDriven/Abilities
   DataDriven/Units
   DataDriven/Teams

.. toctree::
   :caption: Unsorted
   :maxdepth: 1

   FogOfWar/index
   Minimaps/index
   Level/index
   MachineLearning/index

.. toctree::
   :caption: Online

   Online/Multiplayer
   Online/MMO
   Online/Competitive
   Online/Cheating

.. toctree::
   :caption: Developer Guide

   Developer/Release
   Developer/Docs
   Developer/Compilation
   Developer/Shaders/index
   Developer/Doxygen
   Developer/Contribution
   Developer/ci
   generated_api/GamekitAPI


Indices and tables
~~~~~~~~~~~~~~~~~~

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
