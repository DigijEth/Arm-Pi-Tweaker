// Simple test to view the UI layout
use anyhow::Result;

slint::slint! {
    import { Button, VerticalBox, HorizontalBox, GroupBox, ScrollView } from "std-widgets.slint";

    export component TestWindow inherits Window {
        title: "LCCE Layout Test";
        width: 1000px;
        height: 700px;
        background: white;
        
        VerticalBox {
            // Header
            Rectangle {
                height: 60px;
                background: blue;
            }
            
            // Main content
            ScrollView {
                VerticalBox {
                    padding: 24px;
                    spacing: 24px;
                    
                    Rectangle {
                        height: 40px;
                        background: lightgray;
                    }
                    
                    GroupBox {
                        title: "Core Modules";
                        
                        GridLayout {
                            spacing: 16px;
                            
                            Rectangle {
                                row: 0;
                                col: 0;
                                width: 200px;
                                height: 120px;
                                background: lightblue;
                                border-radius: 8px;
                                border-width: 2px;
                                border-color: blue;
                            }
                            
                            Rectangle {
                                row: 0;
                                col: 1;
                                width: 200px;
                                height: 120px;
                                background: lightgreen;
                                border-radius: 8px;
                                border-width: 2px;
                                border-color: green;
                            }
                            
                            Rectangle {
                                row: 1;
                                col: 0;
                                width: 200px;
                                height: 120px;
                                background: lightyellow;
                                border-radius: 8px;
                                border-width: 2px;
                                border-color: orange;
                            }
                            
                            Rectangle {
                                row: 1;
                                col: 1;
                                width: 200px;
                                height: 120px;
                                background: lightpink;
                                border-radius: 8px;
                                border-width: 2px;
                                border-color: red;
                            }
                        }
                    }
                    
                    GroupBox {
                        title: "Advanced Tools";
                        
                        Rectangle {
                            width: 400px;
                            height: 120px;
                            background: lightcyan;
                            border-radius: 8px;
                            border-width: 2px;
                            border-color: darkblue;
                        }
                    }
                }
            }
            
            // Status bar
            Rectangle {
                height: 30px;
                background: darkgray;
            }
        }
    }
}

fn main() -> Result<()> {
    env_logger::init();
    
    let ui = TestWindow::new()?;
    ui.run()?;
    
    Ok(())
}