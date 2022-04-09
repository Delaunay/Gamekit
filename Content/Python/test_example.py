import unreal

import automation_test.unittest_utilities as util


run = util.TestRunner()


@run.add_test
def test_something():
    unreal.log("logging")
    print("printing")
    util.expect_false(lambda: False)


@run.add_test
@util.test_on_linux
def test_something_linux():
    pass


run.run_all()
