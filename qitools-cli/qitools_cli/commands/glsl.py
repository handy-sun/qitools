"""GLSL conversion commands."""

import click
from pathlib import Path
from qitools_cli.core.glsl_converter import to_cpp_string_literal
from qitools_cli.utils.output import print_success, print_error


@click.group(name="glsl")
def glsl_group():
    """GLSL shader conversion tools."""
    pass


@glsl_group.command(name="to-cpp")
@click.option("--input", "-i", "input_file", type=click.Path(exists=True), help="Input GLSL file")
@click.option("--output", "-o", "output_file", type=click.Path(), default="-", help="Output file (- for stdout)")
def glsl_to_cpp(input_file: str, output_file: str):
    """Convert GLSL shader to C++ string literal."""
    try:
        # Read input
        if input_file:
            source = Path(input_file).read_text()
        else:
            import sys
            source = sys.stdin.read()

        # Convert
        result = to_cpp_string_literal(source)

        # Output
        if output_file == "-":
            click.echo(result)
        else:
            Path(output_file).write_text(result)
            print_success(f"Converted to {output_file}")
    except Exception as e:
        print_error(f"Error: {e}")
        raise click.Exit(1)
