extern crate proc_macro;
use proc_macro::TokenStream;

use quote::format_ident;
use quote::quote;

#[proc_macro]
pub fn csr_read(tokens: TokenStream) -> TokenStream {
    let register = format_ident!("{}", tokens.to_string());
    let stream = quote!({
        let mut x: u32 = core::u32::MAX;
        unsafe {
            asm!("csrr {0}, {register}", out(reg) x, register = const #register);
        }
        x
    });

    TokenStream::from(stream)
}

#[proc_macro]
pub fn csr_write(tokens: TokenStream) -> TokenStream {
    let args: Vec<_> = tokens
        .to_string()
        .split(',')
        .map(|s| s.trim().to_owned())
        .collect();
    let register = format_ident!("{}", args[0]);
    let val = format_ident!("{}", args[1]);
    let stream = quote!({
        unsafe {
            asm!("csrw {register}, {val}", register = const #register, val = in(reg) #val);
        }
    });

    TokenStream::from(stream)
}

#[proc_macro]
pub fn csr_set(tokens: TokenStream) -> TokenStream {
    let args: Vec<_> = tokens
        .to_string()
        .split(',')
        .map(|s| s.trim().to_owned())
        .collect();
    let register = format_ident!("{}", args[0]);
    let val = format_ident!("{}", args[1]);
    let stream = quote!({
        unsafe {
            asm!("csrs {register}, {val}", register = const #register, val = in(reg) #val);
        }
    });

    TokenStream::from(stream)
}
