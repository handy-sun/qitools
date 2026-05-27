# qitools-cli

CLI tools for common development tasks - ported from qitools GUI.

## Installation

```bash
# Install dependencies
uv sync

# Or install in development mode
uv pip install -e .
```

## Usage

```bash
# Show help
uv run qitools --help

# Or using python -m
uv run python -m qitools_cli --help
```

## Available Commands

### GLSL Conversion
```bash
# Convert GLSL to C++ string literal
echo "void main() { }" | uv run qitools glsl to-cpp
uv run qitools glsl to-cpp --input shader.frag --output shader.cpp
```

### C++ Code Generation
```bash
# Generate Qt Property code
uv run qitools cppgen property --type int --name count --notify --prefix m_
```

### Encoding Conversion
```bash
# Detect file encoding
uv run qitools codec detect --input file.txt

# Convert encoding
uv run qitools codec convert --input file.txt --from utf-8 --to gbk --output out.txt
```

### Color Conversion
```bash
# Parse and convert color formats
uv run qitools color parse --hex "#FF0000" --format all
uv run qitools color parse --rgb "255,0,0" --format hex
```

### Image Conversion
```bash
# Convert image format
uv run qitools image convert --input img.png --format jpg --output img.jpg

# Resize image
uv run qitools image resize --input img.png --width 800 --height 600 --output resized.png
```

### Download
```bash
# Download file
uv run qitools download file --url https://example.com/file.zip --output file.zip
```

### Audio Metadata
```bash
# Get audio metadata
uv run qitools audio metadata --file song.mp3

# Set audio metadata
uv run qitools audio metadata --file song.mp3 --set-title "New Title"

# Extract cover art
uv run qitools audio extract-cover --file song.mp3 --output cover.jpg
```

## Development

```bash
# Run tests
uv run pytest tests/ -v

# Run linter
uv run ruff check .

# Format code
uv run ruff format .
```

## License

MIT
