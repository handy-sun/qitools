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


def test_to_cpp_string_literal_special_chars():
    """Test special characters are properly escaped."""
    # Test with quotes
    source = 'string s = "hello";'
    result = to_cpp_string_literal(source)
    assert '\\"hello\\"' in result

    # Test with backslashes
    source = 'string s = "path\\to\\file";'
    result = to_cpp_string_literal(source)
    assert '\\\\to\\\\file' in result


def test_to_cpp_string_literal_long_file():
    """Test conversion of a long GLSL file."""
    # Generate a long GLSL source
    lines = ['#version 330 core']
    for i in range(100):
        lines.append(f'uniform float value_{i};')
    lines.append('void main() { gl_FragColor = vec4(1.0); }')
    source = '\n'.join(lines)

    result = to_cpp_string_literal(source)
    result_lines = result.split('\n')

    # Should have same number of lines as input
    assert len(result_lines) == len(lines)

    # First line should contain version
    assert '#version 330 core' in result_lines[0]


def test_to_cpp_string_literal_single_line():
    """Test single line GLSL code."""
    source = 'void main() {}'
    result = to_cpp_string_literal(source)
    assert result == '"void main() {}\\n"'
