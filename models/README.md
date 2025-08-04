# Rocky AI Models

This directory contains AI models for the Rocky chat tab in Arm-Pi Tweaker.

## Current Models

### Google Gemma 3 1B Instruct (Q4_K_M)
- **File**: `gemma-3-1b-it-Q4_K_M.gguf`
- **Size**: 769 MB
- **Parameters**: ~1 billion
- **Quantization**: Q4_K_M (4-bit quantization, good balance of speed and quality)
- **Context Window**: 32K tokens
- **Description**: Instruction-tuned conversational AI model optimized for chat interactions

## Usage in Rocky Tab

1. Launch Arm-Pi Tweaker
2. Navigate to the "🤖 Rocky" tab
3. Click "Browse" or "Load Model"
4. Select the model file: `/home/snake/Rocky/models/gemma-3-1b-it-Q4_K_M.gguf`
5. Adjust settings as needed:
   - Temperature: 0.7 (balanced creativity/consistency)
   - Top-p: 0.9 (good diversity)
   - Context Size: 2048-8192 tokens (adjust based on performance)
6. Start chatting!

## Model Information

This model is:
- ✅ Recommended by llama.cpp developers
- ✅ Optimized for ARM64 architecture (Orange Pi 5+)
- ✅ Instruction-tuned for conversations
- ✅ Small enough for local inference
- ✅ Good performance/quality balance

## Adding More Models

To add more models:
1. Download GGUF format models from Hugging Face or other sources
2. Place them in this `models/` directory
3. Select them in the Rocky tab model browser

Popular GGUF model sources:
- https://huggingface.co/models?library=gguf&sort=trending
- https://huggingface.co/ggml-org
- https://huggingface.co/collections/ggml-org/gemma-3-67d126315ac810df1ad9e913
