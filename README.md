# CppCodeGen

Small c++ code generator utility

## Usage

Call from the command line with a specification file containing definitions of enums to be generated with a config file

```bash
$ codegen enums.cg.json cg.config.json
```

### Specification file

The specification file contains the definitions of all enums to be generated

```json
{
  "headerRoot": "../include",
  "codeRoot": "../src",
  "enums": [
    {
      "name": "Verbosity",
      "description": "Represents the verbosity of logging output",
      "headerPath": "./myproject",
      "codePath": "./myproject",
      "includePath": "myproject",
      "namespace": "myproject",
      "underlyingType": "int",
      "default": "Normal",
      "items": [
        {
          "name": "Quiet",
          "value": 0,
          "string": "Quite",
          "alts": [ "quite", "QUITE" ]
        },
        {
          "name": "Minimal",
          "value": 1,
          "string": "Minimal"
        },
        {
          "name": "Normal",
          "value": 2,
          "string": "Normal"
        },
        {
          "name": "Detailed",
          "value": 3,
          "string": "Detailed"
        },
        {
          "name": "Diagnostic",
          "value": 4,
          "string": "Diagnostic"
        }
      ]
    }
  ]
}
```

#### Top Level Fields

* `headerRoot` - [optional] The root directory for header files. This is prepended to the `headerPath` for each enum
* `codeRoot` - [optional] The root directory for code files. This is prepended to the `codePath` for each enum
* `enums` - List of all enums that will be generated

#### Enum Fields

* `name` - Class name of the template
* `description` - [optional] Description of the enum
* `headerPath` - Path to the header file to be generated
* `codePath` - Path to the code file to be generated
* `includePath` - Path to the header file to be included in other files
* `namespace` - Namespace in which enums are enclosed
* `underlyingType` - [optional] The underlying type of the enum. Defaults to `int`
* `default` - [optional] The default value of the enum, this should match the `name` field of one of the items
* `items` - List of all items in the enum

#### Item Fields

* `name` - Name of the enum item (must be unique)
* `value` - [optional] Value of the enum item
* `string` - String representation of the enum item
* `alts` - [optional] List of alternative string representations of the enum item

Note: All paths are resolved relative to the directory containing the specification file

### Config File

The config file provides the path to the templates that will be used for generating the code

```json
{
  "templates": "./templates",
  "enums": {
    "headerTemplate": "enum.h.inja",
    "codeTemplate": "enum.cpp.inja"
  }
}
```

* `templates` - The path to the directory containing the templates
* `enums::headerTemplate` - The name of the template file to use for generating the header file
* `enums::codeTemplate` - The name of the template file to use for generating the code file

### Templates

Templates are written in the [inja](https://github.com/pantor/inja) templating language. Variables are passed in 
directly from the specification file

#### Simple Example Header

```inja
// Generated enum header
#pragma once

#include <ostream>

namespace {{ enum.namespace }}
{

enum class {{ enum.name }}
{
{% for item in enum.items %}
    {{ item.name }}{% if not loop.last %},{% endif %}
{% endfor %}
};

std::ostream & operator<<(std::ostream & os, {{ enum.name }} value);

} // namespace {{ enum.namespace }}
```

#### Simple Example Code

```inja
// Generated enum code
#include "{{ enum.includePath }}/{{ enum.name }}.h"

namespace {{ enum.namespace }}
{

std::ostream & operator<<(std::ostream & os, {{ enum.name }} value)
{
    switch (value)
    {
{% for item in enum.items %}
        case {{ enum.name }}::{{ item.name }}: return os << "{{ item.string }}";
{% endfor %}
    }

    return os;
}

} // namespace {{ enum.namespace }}
```

More examples are available in the [examples](examples) directory