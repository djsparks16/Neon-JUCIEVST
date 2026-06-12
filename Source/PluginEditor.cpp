#include "PluginEditor.h"
#include "Modulation.h"

namespace
{
    constexpr auto bg0 = 0xff070810u;
    constexpr auto bg1 = 0xff101220u;
    constexpr auto glass = 0xaa171a2cu;
    constexpr auto glassHi = 0x55264fffau;
    constexpr auto cyan = 0xff20f7ffu;
    constexpr auto magenta = 0xffff2bd6u;
    constexpr auto violet = 0xff8a5cffu;
    constexpr auto lime = 0xff7cff4fu;
    constexpr auto amber = 0xffffb020u;

    juce::Colour neonForName (const juce::String& name)
    {
        const auto n = name.toLowerCase();
        if (n.contains ("osc a") || n.contains ("lfo 1")) return juce::Colour (cyan);
        if (n.contains ("osc b") || n.contains ("lfo 2")) return juce::Colour (magenta);
        if (n.contains ("filter") || n.contains ("matrix")) return juce::Colour (lime);
        if (n.contains ("fx") || n.contains ("dist") || n.contains ("delay") || n.contains ("reverb")) return juce::Colour (violet);
        if (n.contains ("env") || n.contains ("sub") || n.contains ("out")) return juce::Colour (amber);
        return juce::Colour (cyan);
    }

    void drawGlassPanel (juce::Graphics& g, juce::Rectangle<float> r, juce::Colour accent, float corner = 14.0f)
    {
        juce::DropShadow (accent.withAlpha (0.28f), 18, { 0, 0 }).drawForRectangle (g, r.toNearestInt());

        juce::ColourGradient fill (juce::Colour (glassHi), r.getTopLeft(), juce::Colour (glass), r.getBottomRight(), false);
        fill.addColour (0.45, juce::Colour (0x9920253au));
        g.setGradientFill (fill);
        g.fillRoundedRectangle (r, corner);

        g.setColour (juce::Colour (0x18ffffff));
        g.drawRoundedRectangle (r.reduced (1.0f), corner, 1.0f);

        g.setColour (accent.withAlpha (0.70f));
        g.drawRoundedRectangle (r.reduced (0.5f), corner, 1.4f);

        auto top = r.withHeight (3.0f).reduced (12.0f, 0.0f);
        juce::ColourGradient glow (accent.withAlpha (0.0f), top.getTopLeft(), accent.withAlpha (0.92f), top.getCentre(), false);
        glow.addColour (1.0, accent.withAlpha (0.0f));
        g.setGradientFill (glow);
        g.fillRoundedRectangle (top, 2.0f);

        // Stained-glass leading lines: restrained geometry, no bitmap assets required.
        juce::Path shards;
        shards.startNewSubPath (r.getX() + r.getWidth() * 0.08f, r.getBottom() - 1.0f);
        shards.lineTo (r.getX() + r.getWidth() * 0.34f, r.getY() + 1.0f);
        shards.startNewSubPath (r.getX() + r.getWidth() * 0.67f, r.getY() + 1.0f);
        shards.lineTo (r.getRight() - r.getWidth() * 0.12f, r.getBottom() - 1.0f);
        g.setColour (accent.withAlpha (0.11f));
        g.strokePath (shards, juce::PathStrokeType (1.0f));
    }
}

//==============================================================================
// NeonObsidianLookAndFeel
//==============================================================================
NeonObsidianLookAndFeel::NeonObsidianLookAndFeel()
{
    setColourScheme (juce::LookAndFeel_V4::getMidnightColourScheme());
    setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (cyan));
    setColour (juce::Slider::thumbColourId,            juce::Colour (magenta));
    setColour (juce::Slider::trackColourId,            juce::Colour (cyan));
    setColour (juce::Slider::textBoxTextColourId,      juce::Colour (0xffe8fbff));
    setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0x55111322));
    setColour (juce::Slider::textBoxOutlineColourId,   juce::Colour (0x6634d2eb));
    setColour (juce::ComboBox::backgroundColourId,     juce::Colour (0x88101624));
    setColour (juce::ComboBox::textColourId,           juce::Colour (0xffe8fbff));
    setColour (juce::ComboBox::outlineColourId,        juce::Colour (cyan).withAlpha (0.65f));
    setColour (juce::PopupMenu::backgroundColourId,    juce::Colour (0xff0c0f1a));
    setColour (juce::PopupMenu::textColourId,          juce::Colour (0xffe8fbff));
    setColour (juce::TextButton::buttonColourId,       juce::Colour (0x88101624));
    setColour (juce::TextButton::textColourOffId,      juce::Colour (0xffe8fbff));
    setColour (juce::TabbedComponent::backgroundColourId, juce::Colour (bg0));
}

void NeonObsidianLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                                float sliderPosProportional, float rotaryStartAngle,
                                                float rotaryEndAngle, juce::Slider& slider)
{
    auto b = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (7.0f);
    const auto radius = juce::jmin (b.getWidth(), b.getHeight()) * 0.5f;
    const auto centre = b.getCentre();
    const auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
    auto accent = slider.findColour (juce::Slider::rotarySliderFillColourId);

    juce::Path shadowPath;
    shadowPath.addEllipse (b);
    juce::DropShadow (accent.withAlpha (0.45f), 16, { 0, 0 }).drawForPath (g, shadowPath);

    juce::ColourGradient body (juce::Colour (0xff232842), b.getTopLeft(), juce::Colour (0xff080a12), b.getBottomRight(), false);
    body.addColour (0.25, juce::Colour (0xff303754));
    g.setGradientFill (body);
    g.fillEllipse (b);

    g.setColour (juce::Colour (0x30ffffff));
    g.drawEllipse (b.reduced (1.0f), 1.0f);

    juce::Path bgArc, valueArc;
    const auto arcRadius = radius - 4.0f;
    bgArc.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f, rotaryStartAngle, rotaryEndAngle, true);
    valueArc.addCentredArc (centre.x, centre.y, arcRadius, arcRadius, 0.0f, rotaryStartAngle, angle, true);
    g.setColour (juce::Colour (0x55252b45));
    g.strokePath (bgArc, juce::PathStrokeType (4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    g.setColour (accent.withAlpha (0.92f));
    g.strokePath (valueArc, juce::PathStrokeType (4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

    juce::Path pointer;
    pointer.addRoundedRectangle (-1.5f, -radius + 10.0f, 3.0f, radius * 0.42f, 1.5f);
    pointer.applyTransform (juce::AffineTransform::rotation (angle).translated (centre.x, centre.y));
    g.setColour (juce::Colour (0xfff4fdff));
    g.fillPath (pointer);

    auto hi = b.reduced (b.getWidth() * 0.22f, b.getHeight() * 0.18f).translated (-b.getWidth() * 0.08f, -b.getHeight() * 0.12f);
    g.setColour (juce::Colour (0x18ffffff));
    g.fillEllipse (hi);
}

void NeonObsidianLookAndFeel::drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                                                float sliderPos, float minSliderPos, float maxSliderPos,
                                                const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    if (style != juce::Slider::LinearHorizontal)
    {
        LookAndFeel_V4::drawLinearSlider (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        return;
    }

    auto track = juce::Rectangle<float> ((float) x, (float) y + height * 0.44f, (float) width, 5.0f).reduced (2.0f, 0.0f);
    auto accent = slider.findColour (juce::Slider::trackColourId);
    g.setColour (juce::Colour (0x7722283e));
    g.fillRoundedRectangle (track, 3.0f);
    g.setColour (accent.withAlpha (0.88f));
    g.fillRoundedRectangle (track.withRight ((float) sliderPos), 3.0f);
    g.setColour (juce::Colour (0xfff4fdff));
    g.fillEllipse (sliderPos - 5.0f, track.getCentreY() - 5.0f, 10.0f, 10.0f);
}

void NeonObsidianLookAndFeel::drawButtonBackground (juce::Graphics& g, juce::Button& button,
                                                    const juce::Colour&, bool over, bool down)
{
    auto b = button.getLocalBounds().toFloat().reduced (1.0f);
    auto accent = down ? juce::Colour (magenta) : (over ? juce::Colour (lime) : juce::Colour (cyan));
    drawGlassPanel (g, b, accent, 9.0f);
}

void NeonObsidianLookAndFeel::drawComboBox (juce::Graphics& g, int width, int height, bool isButtonDown,
                                            int, int, int, int, juce::ComboBox&)
{
    auto b = juce::Rectangle<float> (0, 0, (float) width, (float) height).reduced (0.5f);
    drawGlassPanel (g, b, isButtonDown ? juce::Colour (magenta) : juce::Colour (violet), 8.0f);
    juce::Path arrow;
    arrow.addTriangle ((float) width - 18.0f, height * 0.42f, (float) width - 8.0f, height * 0.42f, (float) width - 13.0f, height * 0.64f);
    g.setColour (juce::Colour (0xffe8fbff));
    g.fillPath (arrow);
}

void NeonObsidianLookAndFeel::positionComboBoxText (juce::ComboBox&, juce::Label& label)
{
    label.setBounds (6, 1, label.getParentWidth() - 24, label.getParentHeight() - 2);
    label.setFont (juce::FontOptions (12.0f, juce::Font::bold));
    label.setJustificationType (juce::Justification::centredLeft);
}

void NeonObsidianLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                                bool over, bool down)
{
    auto b = button.getLocalBounds().toFloat().reduced (2.0f);
    auto accent = button.getToggleState() ? juce::Colour (lime) : juce::Colour (magenta).withAlpha (0.55f);
    if (over || down) accent = accent.brighter (0.25f);
    drawGlassPanel (g, b, accent, b.getHeight() * 0.45f);

    auto pill = b.reduced (5.0f, 5.0f);
    const auto d = pill.getHeight();
    auto knobX = button.getToggleState() ? pill.getRight() - d : pill.getX();
    g.setColour (button.getToggleState() ? juce::Colour (0xfff4fdff) : juce::Colour (0xff626b86));
    g.fillEllipse (knobX, pill.getY(), d, d);
}



//==============================================================================
// ControlPage
//==============================================================================
ControlPage::Section& ControlPage::section (const juce::String& title)
{
    sections.push_back (std::make_unique<Section>());
    sections.back()->title = title;
    return *sections.back();
}

void ControlPage::knob (Section& s, const juce::String& paramID, const juce::String& labelText)
{
    auto c = std::make_unique<Control>();
    c->width = 72;

    c->slider = std::make_unique<juce::Slider>();
    c->slider->setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    c->slider->setTextBoxStyle (juce::Slider::TextBoxBelow, false, 64, 15);
    c->slider->setColour (juce::Slider::rotarySliderFillColourId, neonForName (s.title));
    c->slider->setColour (juce::Slider::thumbColourId, neonForName (s.title));
    c->slider->setColour (juce::Slider::trackColourId, neonForName (s.title));
    addAndMakeVisible (*c->slider);
    c->sAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        apvts, paramID, *c->slider);

    c->label.setText (labelText, juce::dontSendNotification);
    c->label.setJustificationType (juce::Justification::centred);
    c->label.setColour (juce::Label::textColourId, juce::Colour (0xffcfefff));
    c->label.setFont (juce::FontOptions (11.2f, juce::Font::bold));
    addAndMakeVisible (c->label);

    s.controls.push_back (std::move (c));
}

void ControlPage::combo (Section& s, const juce::String& paramID, const juce::String& labelText)
{
    auto c = std::make_unique<Control>();
    c->width = 104;

    c->box = std::make_unique<juce::ComboBox>();
    c->box->setColour (juce::ComboBox::outlineColourId, neonForName (s.title));
    if (auto* choice = dynamic_cast<juce::AudioParameterChoice*> (apvts.getParameter (paramID)))
        c->box->addItemList (choice->choices, 1);
    addAndMakeVisible (*c->box);
    c->cAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
        apvts, paramID, *c->box);

    c->label.setText (labelText, juce::dontSendNotification);
    c->label.setJustificationType (juce::Justification::centred);
    c->label.setColour (juce::Label::textColourId, juce::Colour (0xffcfefff));
    c->label.setFont (juce::FontOptions (11.2f, juce::Font::bold));
    addAndMakeVisible (c->label);

    s.controls.push_back (std::move (c));
}

void ControlPage::toggle (Section& s, const juce::String& paramID, const juce::String& labelText)
{
    auto c = std::make_unique<Control>();
    c->width = 56;

    c->toggle = std::make_unique<juce::ToggleButton>();
    c->toggle->setColour (juce::ToggleButton::tickColourId, neonForName (s.title));
    addAndMakeVisible (*c->toggle);
    c->bAtt = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        apvts, paramID, *c->toggle);

    c->label.setText (labelText, juce::dontSendNotification);
    c->label.setJustificationType (juce::Justification::centred);
    c->label.setColour (juce::Label::textColourId, juce::Colour (0xffcfefff));
    c->label.setFont (juce::FontOptions (11.2f, juce::Font::bold));
    addAndMakeVisible (c->label);

    s.controls.push_back (std::move (c));
}

void ControlPage::paint (juce::Graphics& g)
{
    juce::ColourGradient bg (juce::Colour (bg0), 0.0f, 0.0f,
                             juce::Colour (bg1), (float) getWidth(), (float) getHeight(), false);
    bg.addColour (0.52, juce::Colour (0xff12142a));
    g.setGradientFill (bg);
    g.fillAll();

    // Faint neon glass shards in the page background.
    g.setColour (juce::Colour (cyan).withAlpha (0.055f));
    for (int i = 0; i < getWidth(); i += 190)
        g.drawLine ((float) i, 0.0f, (float) i + 110.0f, (float) getHeight(), 1.0f);
    g.setColour (juce::Colour (magenta).withAlpha (0.045f));
    for (int i = 90; i < getWidth(); i += 230)
        g.drawLine ((float) i, (float) getHeight(), (float) i + 80.0f, 0.0f, 1.0f);

    for (auto& s : sections)
    {
        auto r = s->bounds.toFloat();
        auto accent = neonForName (s->title);
        drawGlassPanel (g, r, accent, 14.0f);

        g.setColour (accent.withAlpha (0.20f));
        g.fillRoundedRectangle (r.reduced (8.0f).withHeight (18.0f), 7.0f);

        g.setColour (juce::Colour (0xffeefcff));
        g.setFont (juce::FontOptions (12.8f, juce::Font::bold));
        auto titleArea = s->bounds;
        g.drawText (s->title, titleArea.removeFromTop (24).reduced (12, 3),
                    juce::Justification::left);
    }
}

void ControlPage::resized()
{
    const int gap = 8, rowH = 142, pad = 8;
    int x = pad, y = pad;

    for (auto& s : sections)
    {
        int w = 16;
        for (auto& c : s->controls)
            w += c->width + 4;

        if (x + w > getWidth() - pad && x > pad)
        {
            x = pad;
            y += rowH + gap;
        }

        s->bounds = { x, y, w, rowH };
        x += w + gap;

        auto inner = s->bounds.reduced (8).withTrimmedTop (20);
        int cx = inner.getX();

        for (auto& c : s->controls)
        {
            juce::Rectangle<int> cell (cx, inner.getY(), c->width, inner.getHeight());
            cx += c->width + 4;

            c->label.setBounds (cell.removeFromBottom (15));

            if (c->slider != nullptr)
                c->slider->setBounds (cell);
            else if (c->box != nullptr)
                c->box->setBounds (cell.withSizeKeepingCentre (c->width - 4, 24));
            else if (c->toggle != nullptr)
                c->toggle->setBounds (cell.withSizeKeepingCentre (24, 24));
        }
    }
}

//==============================================================================
// MatrixPage
//==============================================================================
MatrixPage::MatrixPage (juce::AudioProcessorValueTreeState& apvts)
{
    for (int s = 1; s <= Mod::numSlots; ++s)
    {
        auto row = std::make_unique<Row>();
        const auto n = juce::String (s);

        row->src.addItemList (Mod::sourceNames, 1);
        row->dst.addItemList (Mod::destNames, 1);
        addAndMakeVisible (row->src);
        addAndMakeVisible (row->dst);

        row->amt.setSliderStyle (juce::Slider::LinearHorizontal);
        row->amt.setTextBoxStyle (juce::Slider::TextBoxRight, false, 56, 18);
        row->amt.setColour (juce::Slider::trackColourId, juce::Colour (lime));
        addAndMakeVisible (row->amt);

        row->srcAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
            apvts, "mod" + n + "Src", row->src);
        row->dstAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (
            apvts, "mod" + n + "Dst", row->dst);
        row->amtAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
            apvts, "mod" + n + "Amt", row->amt);

        rows.push_back (std::move (row));
    }
}

void MatrixPage::paint (juce::Graphics& g)
{
    juce::ColourGradient bg (juce::Colour (bg0), 0.0f, 0.0f,
                             juce::Colour (0xff11142a), (float) getWidth(), (float) getHeight(), false);
    g.setGradientFill (bg);
    g.fillAll();

    auto header = getLocalBounds().reduced (10).removeFromTop (34).toFloat();
    drawGlassPanel (g, header, juce::Colour (lime), 13.0f);

    g.setColour (juce::Colour (0xffeefcff));
    g.setFont (juce::FontOptions (12.5f, juce::Font::bold));
    g.drawText ("SOURCE", 24, 14, 150, 20, juce::Justification::left);
    g.drawText ("DESTINATION", 194, 14, 150, 20, juce::Justification::left);
    g.drawText ("AMOUNT", 364, 14, 150, 20, juce::Justification::left);

    int y = 48;
    for (size_t i = 0; i < rows.size(); ++i)
    {
        auto r = juce::Rectangle<float> (10.0f, (float) y, (float) getWidth() - 20.0f, 31.0f);
        drawGlassPanel (g, r, (i % 2 == 0) ? juce::Colour (cyan) : juce::Colour (magenta), 10.0f);
        y += 36;
    }
}

void MatrixPage::resized()
{
    int y = 51;
    for (auto& row : rows)
    {
        row->src.setBounds (18, y, 150, 24);
        row->dst.setBounds (188, y, 150, 24);
        row->amt.setBounds (358, y, juce::jmax (200, getWidth() - 386), 24);
        y += 36;
    }
}

//==============================================================================
// Editor
//==============================================================================
ObsidianAudioProcessorEditor::ObsidianAudioProcessorEditor (ObsidianAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setLookAndFeel (&lnf);

    synthPage  = new ControlPage (processor.apvts);
    fxPage     = new ControlPage (processor.apvts);
    matrixPage = new MatrixPage (processor.apvts);

    buildSynthPage();
    buildFXPage();

    tabs.addTab ("SYNTH",  juce::Colour (0xff14151c), synthPage,  true);
    tabs.addTab ("FX",     juce::Colour (0xff14151c), fxPage,     true);
    tabs.addTab ("MATRIX", juce::Colour (0xff14151c), matrixPage, true);
    addAndMakeVisible (tabs);

    addAndMakeVisible (loadWtButton);
    addAndMakeVisible (savePresetBtn);
    addAndMakeVisible (loadPresetBtn);
    addAndMakeVisible (initBtn);

    loadWtButton.onClick  = [this] { loadWavetableClicked(); };
    savePresetBtn.onClick = [this] { savePresetClicked(); };
    loadPresetBtn.onClick = [this] { loadPresetClicked(); };
    initBtn.onClick       = [this] { initPatchClicked(); };

    setSize (1100, 660);
    setResizable (true, true);
    setResizeLimits (900, 560, 1800, 1100);
}

ObsidianAudioProcessorEditor::~ObsidianAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void ObsidianAudioProcessorEditor::buildSynthPage()
{
    auto& page = *synthPage;

    auto& a = page.section ("OSC A");
    page.combo (a, "oscATable", "Table");
    page.knob  (a, "oscAMorph", "Morph");
    page.combo (a, "oscAWarpMode", "Warp");
    page.knob  (a, "oscAWarpAmt", "Warp Amt");
    page.knob  (a, "oscASemi", "Semi");
    page.knob  (a, "oscAFine", "Fine");
    page.knob  (a, "oscALevel", "Level");

    auto& b = page.section ("OSC B");
    page.combo (b, "oscBTable", "Table");
    page.knob  (b, "oscBMorph", "Morph");
    page.combo (b, "oscBWarpMode", "Warp");
    page.knob  (b, "oscBWarpAmt", "Warp Amt");
    page.knob  (b, "oscBSemi", "Semi");
    page.knob  (b, "oscBFine", "Fine");
    page.knob  (b, "oscBLevel", "Level");

    auto& sn = page.section ("SUB / NOISE");
    page.knob  (sn, "subLevel", "Sub");
    page.combo (sn, "subOct", "Octave");
    page.knob  (sn, "noiseLevel", "Noise");

    auto& u = page.section ("UNISON / VOICE");
    page.knob (u, "uniCount", "Voices");
    page.knob (u, "uniDetune", "Detune");
    page.knob (u, "uniWidth", "Width");
    page.knob (u, "glideTime", "Glide");
    page.knob (u, "bendRange", "Bend");

    auto& f = page.section ("FILTER");
    page.combo (f, "fltModel", "Model");
    page.knob  (f, "cutoff", "Cutoff");
    page.knob  (f, "reso", "Reso");
    page.knob  (f, "fltDrive", "Drive");
    page.knob  (f, "fltEnvAmt", "Env2 Amt");

    auto& e1 = page.section ("ENV 1 (AMP)");
    page.knob (e1, "ampA", "A");
    page.knob (e1, "ampD", "D");
    page.knob (e1, "ampS", "S");
    page.knob (e1, "ampR", "R");

    auto& e2 = page.section ("ENV 2");
    page.knob (e2, "env2A", "A");
    page.knob (e2, "env2D", "D");
    page.knob (e2, "env2S", "S");
    page.knob (e2, "env2R", "R");

    auto& l1 = page.section ("LFO 1");
    page.combo  (l1, "lfo1Shape", "Shape");
    page.knob   (l1, "lfo1Rate", "Rate");
    page.toggle (l1, "lfo1Sync", "Sync");
    page.combo  (l1, "lfo1Div", "Div");
    page.knob   (l1, "lfo1Cut", "> Cutoff");

    auto& l2 = page.section ("LFO 2");
    page.combo  (l2, "lfo2Shape", "Shape");
    page.knob   (l2, "lfo2Rate", "Rate");
    page.toggle (l2, "lfo2Sync", "Sync");
    page.combo  (l2, "lfo2Div", "Div");

    auto& out = page.section ("OUT");
    page.knob (out, "master", "Master");
}

void ObsidianAudioProcessorEditor::buildFXPage()
{
    auto& page = *fxPage;

    auto& d = page.section ("DISTORTION");
    page.toggle (d, "fxDistOn", "On");
    page.knob   (d, "fxDistDrive", "Drive");
    page.knob   (d, "fxDistMix", "Mix");

    auto& c = page.section ("CHORUS");
    page.toggle (c, "fxChorusOn", "On");
    page.knob   (c, "fxChorusRate", "Rate");
    page.knob   (c, "fxChorusDepth", "Depth");
    page.knob   (c, "fxChorusMix", "Mix");

    auto& ph = page.section ("PHASER");
    page.toggle (ph, "fxPhaserOn", "On");
    page.knob   (ph, "fxPhaserRate", "Rate");
    page.knob   (ph, "fxPhaserDepth", "Depth");
    page.knob   (ph, "fxPhaserMix", "Mix");

    auto& dl = page.section ("DELAY");
    page.toggle (dl, "fxDelayOn", "On");
    page.knob   (dl, "fxDelayTime", "Time ms");
    page.knob   (dl, "fxDelayFb", "Feedback");
    page.knob   (dl, "fxDelayMix", "Mix");

    auto& rv = page.section ("REVERB");
    page.toggle (rv, "fxRevOn", "On");
    page.knob   (rv, "fxRevSize", "Size");
    page.knob   (rv, "fxRevDamp", "Damp");
    page.knob   (rv, "fxRevWidth", "Width");
    page.knob   (rv, "fxRevMix", "Mix");

    auto& cp = page.section ("COMPRESSOR");
    page.toggle (cp, "fxCompOn", "On");
    page.knob   (cp, "fxCompThresh", "Thresh");
    page.knob   (cp, "fxCompRatio", "Ratio");
}

//==============================================================================
void ObsidianAudioProcessorEditor::loadWavetableClicked()
{
    chooser = std::make_unique<juce::FileChooser> ("Load wavetable (WAV, 2048-sample frames)",
                                                   juce::File(), "*.wav");
    chooser->launchAsync (juce::FileBrowserComponent::openMode
                            | juce::FileBrowserComponent::canSelectFiles,
        [this] (const juce::FileChooser& fc)
        {
            const auto file = fc.getResult();
            if (file.existsAsFile() && ! processor.loadUserWavetable (file))
                juce::AlertWindow::showMessageBoxAsync (juce::MessageBoxIconType::WarningIcon,
                                                        "Obsidian", "Couldn't read that file.");
        });
}

void ObsidianAudioProcessorEditor::savePresetClicked()
{
    chooser = std::make_unique<juce::FileChooser> ("Save preset", juce::File(), "*.obsn");
    chooser->launchAsync (juce::FileBrowserComponent::saveMode
                            | juce::FileBrowserComponent::canSelectFiles,
        [this] (const juce::FileChooser& fc)
        {
            auto file = fc.getResult();
            if (file == juce::File())
                return;
            if (auto xml = processor.apvts.copyState().createXml())
                xml->writeTo (file.withFileExtension ("obsn"));
        });
}

void ObsidianAudioProcessorEditor::loadPresetClicked()
{
    chooser = std::make_unique<juce::FileChooser> ("Load preset", juce::File(), "*.obsn");
    chooser->launchAsync (juce::FileBrowserComponent::openMode
                            | juce::FileBrowserComponent::canSelectFiles,
        [this] (const juce::FileChooser& fc)
        {
            const auto file = fc.getResult();
            if (! file.existsAsFile())
                return;
            if (auto xml = juce::parseXML (file))
                processor.apvts.replaceState (juce::ValueTree::fromXml (*xml));
        });
}

void ObsidianAudioProcessorEditor::initPatchClicked()
{
    for (auto* param : processor.getParameters())
        if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*> (param))
        {
            ranged->beginChangeGesture();
            ranged->setValueNotifyingHost (ranged->getDefaultValue());
            ranged->endChangeGesture();
        }
}

//==============================================================================
void ObsidianAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::ColourGradient bg (juce::Colour (0xff05060b), 0.0f, 0.0f,
                             juce::Colour (0xff151730), (float) getWidth(), 44.0f, false);
    bg.addColour (0.52, juce::Colour (0xff101226));
    g.setGradientFill (bg);
    g.fillAll();

    auto topGlass = getLocalBounds().removeFromTop (44).toFloat().reduced (6.0f, 5.0f);
    drawGlassPanel (g, topGlass, juce::Colour (cyan), 13.0f);

    g.setColour (juce::Colour (cyan).withAlpha (0.35f));
    g.drawLine (18.0f, 38.0f, 315.0f, 38.0f, 2.0f);
    g.setColour (juce::Colour (magenta).withAlpha (0.35f));
    g.drawLine (64.0f, 6.0f, 226.0f, 38.0f, 1.0f);
    g.setColour (juce::Colour (lime).withAlpha (0.24f));
    g.drawLine (226.0f, 6.0f, 300.0f, 38.0f, 1.0f);

    g.setColour (juce::Colour (0xfff4fdff));
    g.setFont (juce::FontOptions (25.0f, juce::Font::bold));
    g.drawText ("OBSIDIAN", 18, 7, 260, 29, juce::Justification::left);

    g.setColour (juce::Colour (cyan));
    g.setFont (juce::FontOptions (11.5f, juce::Font::bold));
    g.drawText ("NEON GLASS WAVETABLE SYNTH", 163, 16, 250, 18, juce::Justification::left);
}

void ObsidianAudioProcessorEditor::resized()
{
    auto top = getLocalBounds().removeFromTop (44).reduced (8);
    top.removeFromLeft (330); // title space

    initBtn.setBounds       (top.removeFromRight (60).reduced (2));
    loadPresetBtn.setBounds (top.removeFromRight (100).reduced (2));
    savePresetBtn.setBounds (top.removeFromRight (100).reduced (2));
    loadWtButton.setBounds  (top.removeFromRight (130).reduced (2));

    tabs.setBounds (getLocalBounds().withTrimmedTop (44).reduced (4));
}
