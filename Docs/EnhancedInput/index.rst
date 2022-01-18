Enhanced Input
==============

Advantages:

* Reusable Input Set
* Context Specific Inputs
* Takes away input logic out of the Controller/Pawn which can focus on the actual gameplay

Limitations
-----------

* If you require an input event to tick for every frame or periodically this will not work
  An event must occur or an input needs to be actuated to the entire Input system to be triggered.
  Which means if an input is only Mouse-XY then it will only get triggered when something else
  gets triggered.
  Mouse movement alone does not trigger the input system.

References
----------

.. [1]: `UE4 EnhancedInput <https://docs.unrealengine.com/4.27/en-US/InteractiveExperiences/Input/EnhancedInput/>`_
.. [1]: `API Documentation <https://docs.unrealengine.com/4.26/en-US/API/Plugins/EnhancedInput/>`_
