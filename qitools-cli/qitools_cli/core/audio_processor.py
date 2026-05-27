"""Audio processing logic - no UI dependencies."""

from pathlib import Path
from mutagen.mp3 import MP3
from mutagen.id3 import ID3, TIT2, TPE1, TALB, APIC


def get_audio_metadata(file_path: str) -> dict[str, str]:
    """Get audio metadata.

    Args:
        file_path: Path to audio file

    Returns:
        Dictionary of metadata
    """
    audio = MP3(file_path, ID3=ID3)
    meta = {}

    # Basic info
    meta["Duration"] = f"{audio.info.length:.2f}s"
    meta["Sample Rate"] = f"{audio.info.sample_rate} Hz"
    meta["Channels"] = str(audio.info.channels)
    meta["Bitrate"] = f"{audio.info.bitrate // 1000} kbps"

    # ID3 tags
    if audio.tags:
        if audio.tags.get("TIT2"):
            meta["Title"] = str(audio.tags["TIT2"])
        if audio.tags.get("TPE1"):
            meta["Artist"] = str(audio.tags["TPE1"])
        if audio.tags.get("TALB"):
            meta["Album"] = str(audio.tags["TALB"])

    return meta


def set_audio_metadata(
    file_path: str,
    title: str | None = None,
    artist: str | None = None,
    album: str | None = None
) -> None:
    """Set audio metadata.

    Args:
        file_path: Path to audio file
        title: Title to set
        artist: Artist to set
        album: Album to set
    """
    audio = MP3(file_path, ID3=ID3)

    # Create ID3 tag if it doesn't exist
    if audio.tags is None:
        audio.add_tags()

    if title:
        audio.tags.add(TIT2(encoding=3, text=[title]))
    if artist:
        audio.tags.add(TPE1(encoding=3, text=[artist]))
    if album:
        audio.tags.add(TALB(encoding=3, text=[album]))

    audio.save()


def extract_cover(file_path: str, output_file: str) -> None:
    """Extract cover art from audio file.

    Args:
        file_path: Path to audio file
        output_file: Path to save cover image
    """
    audio = MP3(file_path, ID3=ID3)

    if audio.tags is None:
        raise ValueError("No ID3 tags found")

    # Find APIC frame (front cover)
    for key in audio.tags:
        if key.startswith("APIC"):
            apic = audio.tags[key]
            Path(output_file).write_bytes(apic.data)
            return

    raise ValueError("No cover art found")
