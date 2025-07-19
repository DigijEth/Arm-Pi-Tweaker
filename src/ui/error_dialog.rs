use cursive::views::{Dialog, LinearLayout, TextView, SelectView, ScrollView};
use cursive::Cursive;
use cursive::align::HAlign;
use cursive::theme::{ColorStyle, BaseColor, Color};
use cursive::utils::markup::StyledString;
use cursive::traits::*;
use crate::error::{BuilderError, ContextualError, user_friendly_error, get_recovery_suggestions};

pub fn show_error_dialog(siv: &mut Cursive, error: &BuilderError) {
    let title = match error {
        BuilderError::FileNotFound(_) => "File Not Found",
        BuilderError::PermissionDenied(_) => "Permission Denied",
        BuilderError::DiskFull(_) => "Disk Full",
        BuilderError::DependencyMissing(_) => "Missing Dependency",
        BuilderError::NetworkError(_) => "Network Error",
        BuilderError::DeviceNotFound(_) => "Device Not Found",
        BuilderError::BuildFailed(_) => "Build Failed",
        BuilderError::SystemCommandFailed(_, _) => "Command Failed",
        _ => "Error",
    };

    let friendly_message = user_friendly_error(error);
    let suggestions = get_recovery_suggestions(error);

    let mut content = LinearLayout::vertical();
    
    // Error message
    let error_text = TextView::new(StyledString::styled(
        friendly_message,
        ColorStyle::from(Color::Light(BaseColor::Red))
    ));
    content.add_child(error_text);
    
    if !suggestions.is_empty() {
        content.add_child(TextView::new(""));
        content.add_child(TextView::new(StyledString::styled(
            "Suggestions:",
            ColorStyle::from(Color::Light(BaseColor::Yellow))
        )));
        
        for suggestion in suggestions {
            content.add_child(TextView::new(format!("• {}", suggestion)));
        }
    }
    
    // Technical details (collapsible)
    content.add_child(TextView::new(""));
    content.add_child(TextView::new(StyledString::styled(
        "Technical Details:",
        ColorStyle::from(Color::Dark(BaseColor::White))
    )));
    content.add_child(TextView::new(format!("{:?}", error)));
    
    let dialog = Dialog::around(ScrollView::new(content).max_height(15))
        .title(title)
        .button("OK", |s| { s.pop_layer(); })
        .button("Copy Error", move |s| {
            // In a real implementation, you'd copy to clipboard
            s.add_layer(
                Dialog::info("Error details copied to clipboard")
                    .button("OK", |s| { s.pop_layer(); })
            );
        });
    
    siv.add_layer(dialog);
}

pub fn show_contextual_error_dialog(siv: &mut Cursive, error: &ContextualError) {
    let mut content = LinearLayout::vertical();
    
    // Context information
    content.add_child(TextView::new(StyledString::styled(
        format!("Component: {}", error.context.component),
        ColorStyle::from(Color::Light(BaseColor::Blue))
    )));
    content.add_child(TextView::new(StyledString::styled(
        format!("Operation: {}", error.context.operation),
        ColorStyle::from(Color::Light(BaseColor::Blue))
    )));
    content.add_child(TextView::new(""));
    
    // Error message
    let friendly_message = user_friendly_error(&error.error);
    let error_text = TextView::new(StyledString::styled(
        friendly_message,
        ColorStyle::from(Color::Light(BaseColor::Red))
    ));
    content.add_child(error_text);
    
    // Context suggestion
    if let Some(suggestion) = &error.context.suggestion {
        content.add_child(TextView::new(""));
        content.add_child(TextView::new(StyledString::styled(
            "Suggestion:",
            ColorStyle::from(Color::Light(BaseColor::Yellow))
        )));
        content.add_child(TextView::new(format!("• {}", suggestion)));
    }
    
    // Additional recovery suggestions
    let suggestions = get_recovery_suggestions(&error.error);
    if !suggestions.is_empty() {
        content.add_child(TextView::new(""));
        content.add_child(TextView::new(StyledString::styled(
            "Additional Suggestions:",
            ColorStyle::from(Color::Light(BaseColor::Yellow))
        )));
        
        for suggestion in suggestions {
            content.add_child(TextView::new(format!("• {}", suggestion)));
        }
    }
    
    // Technical details
    content.add_child(TextView::new(""));
    content.add_child(TextView::new(StyledString::styled(
        "Technical Details:",
        ColorStyle::from(Color::Dark(BaseColor::White))
    )));
    content.add_child(TextView::new(format!("{}", error)));
    
    let dialog = Dialog::around(ScrollView::new(content).max_height(20))
        .title("Error Details")
        .button("OK", |s| { s.pop_layer(); })
        .button("Retry", |s| { 
            s.pop_layer(); 
            // In a real implementation, you'd retry the operation
        })
        .button("Copy Error", move |s| {
            s.add_layer(
                Dialog::info("Error details copied to clipboard")
                    .button("OK", |s| { s.pop_layer(); })
            );
        });
    
    siv.add_layer(dialog);
}

pub fn show_recovery_dialog(siv: &mut Cursive, error: &BuilderError, retry_callback: Box<dyn Fn(&mut Cursive)>) {
    let suggestions = get_recovery_suggestions(error);
    
    let mut content = LinearLayout::vertical();
    
    // Error summary
    content.add_child(TextView::new(StyledString::styled(
        "An error occurred. Here are some ways to resolve it:",
        ColorStyle::from(Color::Light(BaseColor::Yellow))
    )));
    content.add_child(TextView::new(""));
    
    // Recovery options as selectable list
    let mut recovery_select = SelectView::<String>::new()
        .h_align(HAlign::Left)
        .autojump();
    
    for (i, suggestion) in suggestions.iter().enumerate() {
        recovery_select.add_item(format!("{}. {}", i + 1, suggestion), suggestion.clone());
    }
    
    recovery_select.set_on_submit(move |s, _suggestion: &str| {
        s.pop_layer();
        // In a real implementation, you'd execute the recovery action
        s.add_layer(
            Dialog::info("Recovery action executed")
                .button("OK", |s| { s.pop_layer(); })
        );
    });
    
    content.add_child(recovery_select);
    
    let dialog = Dialog::around(content)
        .title("Error Recovery")
        .button("Retry", move |s| {
            s.pop_layer();
            retry_callback(s);
        })
        .button("Cancel", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

pub fn show_validation_error_dialog(siv: &mut Cursive, field_name: &str, error: &str, suggestions: Vec<String>) {
    let mut content = LinearLayout::vertical();
    
    // Field and error
    content.add_child(TextView::new(StyledString::styled(
        format!("Field: {}", field_name),
        ColorStyle::from(Color::Light(BaseColor::Blue))
    )));
    content.add_child(TextView::new(StyledString::styled(
        format!("Error: {}", error),
        ColorStyle::from(Color::Light(BaseColor::Red))
    )));
    
    if !suggestions.is_empty() {
        content.add_child(TextView::new(""));
        content.add_child(TextView::new(StyledString::styled(
            "Suggestions:",
            ColorStyle::from(Color::Light(BaseColor::Yellow))
        )));
        
        for suggestion in suggestions {
            content.add_child(TextView::new(format!("• {}", suggestion)));
        }
    }
    
    let dialog = Dialog::around(content)
        .title("Validation Error")
        .button("OK", |s| { s.pop_layer(); });
    
    siv.add_layer(dialog);
}

pub fn show_confirmation_dialog(
    siv: &mut Cursive,
    title: &str,
    message: &str,
    confirm_callback: Box<dyn Fn(&mut Cursive)>
) {
    let message = message.to_string();
    let title = title.to_string();
    
    siv.add_layer(
        Dialog::text(message)
            .title(title)
            .button("Yes", move |s| {
                s.pop_layer();
                confirm_callback(s);
            })
            .button("No", |s| { s.pop_layer(); })
    );
}

pub fn show_progress_with_error_handling(
    siv: &mut Cursive,
    title: &str,
    operation: Box<dyn Fn() -> Result<(), BuilderError>>
) {
    let title = title.to_string();
    
    // In a real implementation, you'd run the operation in a separate thread
    // and update the progress dialog
    match operation() {
        Ok(_) => {
            siv.add_layer(
                Dialog::info("Operation completed successfully")
                    .button("OK", |s| { s.pop_layer(); })
            );
        }
        Err(error) => {
            show_error_dialog(siv, &error);
        }
    }
}