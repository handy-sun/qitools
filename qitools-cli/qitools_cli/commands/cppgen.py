"""C++ code generation commands."""

import click
from pathlib import Path
from qitools_cli.core.property_generator import generate_property_code
from qitools_cli.utils.output import print_success, print_error


@click.group(name="cppgen")
def cppgen_group():
    """C++ code generation tools."""
    pass


@cppgen_group.command(name="property")
@click.option("--type", "-t", "prop_type", required=True, help="Property type (e.g., int, QString)")
@click.option("--name", "-n", "prop_name", required=True, help="Property name")
@click.option("--notify", is_flag=True, help="Generate notify signal")
@click.option("--prefix", "-p", default="m_", help="Member variable prefix")
@click.option("--inline", is_flag=True, help="Generate inline code")
@click.option("--output", "-o", "output_file", type=click.Path(), default="-", help="Output file (- for stdout)")
def generate_property(prop_type: str, prop_name: str, notify: bool, prefix: str, inline: bool, output_file: str):
    """Generate Qt Property getter/setter/notify code."""
    try:
        result = generate_property_code(prop_type, prop_name, notify, prefix, inline)

        if output_file == "-":
            click.echo(result)
        else:
            Path(output_file).write_text(result)
            print_success(f"Generated to {output_file}")
    except Exception as e:
        print_error(f"Error: {e}")
        raise click.Exit(1)
