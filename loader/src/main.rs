#![no_main]
#![no_std]

mod elf;

use crate::elf::elf_load;
use uefi::{allocator, prelude::*, data_types::*, table::runtime::ResetType, proto::console::text::{Color, ScanCode, Key}};
use uefi::CString16;
use uefi::fs::{FileSystem, FileSystemResult};
use uefi::prelude::BootServices;
use uefi::proto::media::fs::SimpleFileSystem;
use uefi::table::boot::ScopedProtocol;
use log::{info, error};

#[entry]
fn main(_handle: Handle, mut system_table: SystemTable<Boot>) -> Status {
    uefi::helpers::init().unwrap();
    system_table.stdout().set_color(Color::LightGray, Color::Black);
    system_table.stdout().set_cursor_position(0, 0);
    system_table.stdout().clear();

    const menu: &str =
			"                                       \r\n\
			                                        \r\n\
			╔═══════════Welcome to Hydra═══════════╗\r\n\
			║                                      ║\r\n\
			║ 1. Boot Hydra Normally (Enter)       ║\r\n\
			║ 2. Reboot (r)                        ║\r\n\
			║                                      ║\r\n\
			║ 3. Enter Firmware Settings (Esc)     ║\r\n\
			║                                      ║\r\n\
			║                                      ║\r\n\
			║                                      ║\r\n\
			║ Hydra 0.0.1 (dev)                    ║\r\n\
			╚══════════════════════════════════════╝\r\n\
			                                        \r\n";

    let mut buf = [0; menu.len()];
    let s = CStr16::from_str_with_buf(menu, &mut buf).unwrap();
    system_table.stdout().output_string(s);

    loop {
        let mut events = unsafe { [system_table.stdin().wait_for_key_event().unwrap()] };
        system_table.boot_services()
            .wait_for_event(&mut events)
            .discard_errdata().unwrap();

        let r_key = Char16::try_from('r').unwrap();
        let ret_key = Char16::try_from('\n').unwrap();
        let car_key = Char16::try_from('\r').unwrap();
        match system_table.stdin().read_key().unwrap() {
            Some(key) => match key {
                //Key::Printable(r_key) => system_table.runtime_services().reset(ResetType::COLD, Status::SUCCESS, None),
                Key::Printable(ret_key) => {
                    let path: CString16 = CString16::try_from(r"\efi\boot\hydra").unwrap();
                    let fs: ScopedProtocol<SimpleFileSystem> = system_table.boot_services().get_image_file_system(system_table.boot_services().image_handle()).unwrap();
                    let mut fs = FileSystem::new(fs);
                    let data = fs.read(path.as_ref()).unwrap();
                    info!("{}", data.len());
                    elf_load(data);
                    //return Status::SUCCESS;
                }
                Key::Special(ScanCode::ESCAPE) => {
                    return Status::SUCCESS;
                }

                _ => {}
            }

            _ => {}
        }
    }
}
