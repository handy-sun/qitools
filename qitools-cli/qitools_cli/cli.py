"""Main CLI entry point for qitools."""

import click
from qitools_cli import __version__
from qitools_cli.commands import glsl, cppgen, codec, color, image, download, audio


@click.group()
@click.version_option(version=__version__, prog_name="qitools")
def main():
    """CLI tools for common development tasks."""
    pass


# Register command groups
main.add_command(glsl.glsl_group)
main.add_command(cppgen.cppgen_group)
main.add_command(codec.codec_group)
main.add_command(color.color_group)
main.add_command(image.image_group)
main.add_command(download.download_group)
main.add_command(audio.audio_group)


if __name__ == "__main__":
    main()
