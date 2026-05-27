"""GLSL conversion logic - no UI dependencies."""


def to_cpp_string_literal(source: str) -> str:
    """Convert GLSL source to C++ string literal.

    Args:
        source: GLSL shader source code

    Returns:
        C++ formatted string literal

    Raises:
        ValueError: If source is empty
    """
    if not source:
        raise ValueError("Source cannot be empty")

    # Split into lines
    lines = source.split('\n')

    # Escape each line and wrap in quotes
    escaped_lines = []
    for line in lines:
        # Escape backslashes and quotes
        escaped = line.replace('\\', '\\\\').replace('"', '\\"')
        escaped_lines.append(f'"{escaped}\\n"')

    # Join lines
    return '\n'.join(escaped_lines)
