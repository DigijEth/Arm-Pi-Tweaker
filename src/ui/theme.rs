use cursive::theme::{Theme, Palette, PaletteColor, Color, BaseColor, BorderStyle};
use cursive::Cursive;

pub fn apply_theme(siv: &mut Cursive) {
    let mut theme = Theme::default();
    
    // Set border style
    theme.borders = BorderStyle::Simple;
    
    // Customize palette
    let mut palette = Palette::default();
    
    // Primary colors - solid backgrounds to prevent flickering
    palette[PaletteColor::Background] = Color::Dark(BaseColor::Black); // Solid black background
    palette[PaletteColor::View] = Color::Dark(BaseColor::Black); // Solid black for views
    palette[PaletteColor::Primary] = Color::Light(BaseColor::Blue);
    palette[PaletteColor::Secondary] = Color::Light(BaseColor::Cyan);
    palette[PaletteColor::Tertiary] = Color::Light(BaseColor::Green);
    
    // Highlight colors
    palette[PaletteColor::Highlight] = Color::from_256colors(33); // Blue
    palette[PaletteColor::HighlightInactive] = Color::from_256colors(238); // Light gray
    palette[PaletteColor::HighlightText] = Color::from_256colors(231); // White
    
    // Shadow
    palette[PaletteColor::Shadow] = Color::from_256colors(232); // Very dark gray
    
    // Dialog backgrounds - solid to prevent transparency issues
    palette[PaletteColor::TitlePrimary] = Color::Light(BaseColor::Blue);
    palette[PaletteColor::TitleSecondary] = Color::Dark(BaseColor::Black);
    
    theme.palette = palette;
    
    // Disable shadows to prevent transparency artifacts
    theme.shadow = false;
    
    siv.set_theme(theme);
}