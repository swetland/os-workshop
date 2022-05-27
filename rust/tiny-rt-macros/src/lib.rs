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
