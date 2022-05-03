Release Procedure
================

#. Package the plugin

  .. code-block:: bash

     ../UnrealEngine/Engine/Build/BatchFiles/RunUAT.bat BuildPlugin -Plugin=../Gamekit2/Plugins/Gamekit/Gamekit.uplugin -Package=Gamekit -Rocket


#. Remove unwanted files like tne Intermediate folder and the Gamekit.pbd

#. Send the packaged plugin as a zipfile

  .. note::

      Size limit is around 10 MB for Gitlab, use Google Drive for larger files

  .. code-block:: bash

      curl --request POST --header "PRIVATE-TOKEN: <your_access_token>" \
           --form "file=@Gamekit.zip" "https://gitlab.com/api/v4/projects/28227915/uploads"

      {
          "alt":"Gamekit-release.zip",
          "url":"/uploads/e7e99f9852d83c94dd38c5e5bf9db6e3/Gamekit-release.zip",
          "full_path":"/Delaunay/gamekit/uploads/e7e99f9852d83c94dd38c5e5bf9db6e3/Gamekit-release.zip",
          "markdown":"[Gamekit-release.zip](/uploads/e7e99f9852d83c94dd38c5e5bf9db6e3/Gamekit-release.zip)"
      }

#. Create new release
