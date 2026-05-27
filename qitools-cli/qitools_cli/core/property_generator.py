"""Property code generation logic - no UI dependencies."""


def generate_property_code(
    prop_type: str,
    prop_name: str,
    notify: bool = False,
    prefix: str = "m_",
    inline: bool = False
) -> str:
    """Generate Qt Property getter/setter/notify code.

    Args:
        prop_type: Property type (e.g., int, QString)
        prop_name: Property name
        notify: Whether to generate notify signal
        prefix: Member variable prefix
        inline: Whether to generate inline code

    Returns:
        Generated C++ code
    """
    # Capitalize first letter for method names
    capitalized_name = prop_name[0].upper() + prop_name[1:]

    lines = []

    # Getter
    if inline:
        lines.append(f"{prop_type} {prop_name}() const {{ return {prefix}{prop_name}; }}")
    else:
        lines.append(f"{prop_type} {prop_name}() const;")

    # Setter
    if inline:
        lines.append(f"void set{capitalized_name}(const {prop_type}& {prop_name}) {{")
        lines.append(f"    if ({prefix}{prop_name} != {prop_name}) {{")
        lines.append(f"        {prefix}{prop_name} = {prop_name};")
        if notify:
            lines.append(f"        emit {prop_name}Changed();")
        lines.append("    }")
        lines.append("}")
    else:
        lines.append(f"void set{capitalized_name}(const {prop_type}& {prop_name});")

    # Notify signal (declaration only)
    if notify:
        lines.append("")
        lines.append(f"signals:")
        lines.append(f"    void {prop_name}Changed();")

    return '\n'.join(lines)
