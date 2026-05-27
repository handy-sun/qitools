"""Tests for GLSL conversion."""

from qitools_cli.core.glsl_converter import to_cpp_string_literal


def test_to_cpp_string_literal():
    """Test GLSL to C++ string literal conversion."""
    source = 'void main() { gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); }'
    result = to_cpp_string_literal(source)
    assert '"void main() { gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0); }\\n"' in result


def test_to_cpp_string_literal_multiline():
    """Test multiline GLSL conversion."""
    source = 'void main() {\n    gl_FragColor = vec4(1.0);\n}'
    result = to_cpp_string_literal(source)
    lines = result.split('\n')
    assert len(lines) == 3


def test_to_cpp_string_literal_empty():
    """Test empty source raises error."""
    import pytest
    with pytest.raises(ValueError):
        to_cpp_string_literal('')
