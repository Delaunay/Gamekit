Documentation
=============

You can use sphinx + doxygen to generate documentation for your project.
Doxygen is used to parse C++ code and extract the docstring into a XML.
Sphinx with the breathe and exhale extensions parse the XML and generate the documentation as HTML.

* Sphinx configuration `example <https://github.com/Delaunay/Gamekit/blob/master/Docs/conf.py>`_
* Doxygen configuration `example <https://github.com/Delaunay/Gamekit/blob/master/Docs/Doxyfile.in>`_
* Read the docs configuration `example <https://github.com/Delaunay/Gamekit/blob/master/.readthedocs.yaml>`_

.. note::

   You can use the documentation for personal note taking.
   You will thank your past self.


Continuous Integration
======================

You can setup a CI using github actions.
Epic Games release UnrealEngine containers you can use if you are authenticated.

.. code-block:: yaml

   name: tests

   on: [push]

   # see https://docs.unrealengine.com/4.27/en-US/SharingAndReleasing/Containers/ContainersQuickStart/
   jobs:
   cooking:
      runs-on: ubuntu-latest
      container:
         # Use the slim version, the regular version is too big for github actions
         image: ghcr.io/epicgames/unreal-engine:dev-slim-4.27
         credentials:
         username: ${{ github.actor }}
         password: ${{ secrets.CONTAINER_REGISTRY_PAT }}

      steps:
         - uses: actions/checkout@v1

         - name: Cook
           run: |
             # Sanity check
             ls

             # Change log location to somewhere writtable
             # see https://docs.github.com/en/actions/using-github-hosted-runners/about-github-hosted-runners#docker-container-filesystem
             export uebp_LogFolder=UATLogs

             # Run UAT
             /home/ue4/UnrealEngine/Engine/Build/BatchFiles/RunUAT.sh \
               BuildCookRun                                           \
               -utf8output                                            \
               -platform=Linux                                        \
               -clientconfig=Shipping                                 \
               -serverconfig=Shipping                                 \
               -project=Chessy.uproject                               \
               -noP4 -nodebuginfo -allmaps                            \
               -cook -build -stage -prereqs -pak -archive             \
               -archivedirectory=Cooked/

         - name: Upload
           uses: actions/upload-artifact@v2
           with:
             name: cooked-chessy-linux
             path: Cooked/*


.. note::

   ``CONTAINER_REGISTRY_PAT`` is a github personnal access token with read package permission

.. note::

   Space on the github action worker is limited. You might need to consider self-hosting it.

