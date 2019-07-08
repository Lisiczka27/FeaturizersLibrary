import sys
import json
import re

from DataPipelines import CppToJson
from CommonEnvironment import CommandLine
from DataPipelines.CheckPolicy import Policy

@CommandLine.EntryPoint
@CommandLine.Constraints(
    input=CommandLine.FilenameTypeInfo(
        arity="+",
    ),
)
def EntryPoint(
    input,
    treat_warnings_as_errors=False,
):
    inputs = input
    del input

    class UnsupportedException(Exception):
        pass

    def OnUnsupportedFunc(func, filename, line):
        # TODO: Change this to be more specific.
        # usupported function {} in {} <{}> because type {} is not supported.
        # Display error
        if treat_warnings_as_errors:
            sys.stdout.write("Error: Unsupported function '{}' in {} <{}>\n".format(func, filename, line))
            raise UnsupportedException()
        else:
            sys.stdout.write("Warning: Unsupported function '{}' in {} <{}>\n".format(func, filename, line))

    # ----------------------------------------------------------------------

    try:
        file_dict = {}
        for file_name in inputs:
            file_dict[file_name] = CppToJson.ObtainFunctions(file_name, OnUnsupportedFunc, Policy)
        sys.stdout.write("{}\n".format(json.dumps(file_dict)))
    except UnsupportedException:
        return -1

    return 0

if __name__ == "__main__":
    try: sys.exit(CommandLine.Main())
    except KeyboardInterrupt: pass
