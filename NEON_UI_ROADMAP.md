# Obsidian Neon UI Roadmap

This build starts the visual overhaul: neon palette, glass panels, stained-glass linework, glowing controls, and a darker modern top bar.

## Implemented in this pass

- Added `NeonObsidianLookAndFeel` in `PluginEditor.h/.cpp`.
- Replaced the generic JUCE midnight skin with custom neon rotary knobs.
- Added glass button, combo box, toggle, and horizontal slider drawing.
- Added dark glass page backgrounds with faint stained-glass geometry.
- Added section-specific neon accent colours:
  - Osc A / LFO 1: cyan
  - Osc B / LFO 2: magenta
  - Filter / Matrix: lime
  - Env / Sub / Output: amber
  - FX: violet
- Reworked panel painting to use glowing glass cards instead of flat rectangles.
- Reworked the top bar into a glass header with neon title treatment.
- Reworked matrix page row backgrounds into alternating neon glass strips.

## Next feature/UI passes

1. Add oscillator wavetable displays for Osc A and Osc B.
2. Add visual ADSR editors for Env 1 and Env 2.
3. Add visual LFO editors with editable curves and tempo grid.
4. Add macro controls and macro modulation destinations.
5. Add modulation rings around knobs.
6. Add drag-and-drop modulation source handles.
7. Add preset browser with tags, favourites, author, and category metadata.
8. Add FX cards with reorder handles and animated meters.
9. Add more oscillator warp modes: bend, fold, quantize, PWM, asymmetry, AM, RM, remap.
10. Add more filter models: notch, comb, formant, diode, dirty ladder.

## Design rule

Use Serum-like workflow principles, but keep Obsidian visually and structurally distinct: black volcanic glass, neon stained-glass accents, crystalline panels, and spectral modulation colours.
