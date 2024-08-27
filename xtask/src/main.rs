use std::{
    env, fs,
    path::{Path, PathBuf},
    process::{Command, Stdio},
};
use toml::Table;

fn main() {
    if let Err(e) = try_main() {
        eprintln!("{}", e);
        std::process::exit(1);
    }
}

fn try_main() -> Result<(), Box<dyn std::error::Error>> {
    let task = env::args().nth(1);
    match task.as_deref() {
        Some("kernel") => kernel()?,
        Some("loader") => loader()?,
        Some("dist") => dist()?,
        Some("setup") => setup()?,
        Some("run") => run()?,
        _ => print_help(),
    }

    Ok(())
}

fn setup() -> Result<(), Box<dyn std::error::Error>> {
    fs::copy(project_root().join("config.def.toml"), project_root().join("config.toml"))?;
    
    Ok(())
}

fn run() -> Result<(), Box<dyn std::error::Error>> {
    dist()?;
    
    let config = config();
    let qemu = config["qemu"]["bin"].clone();
    println!("{}", config["qemu"]["bin"].as_str().unwrap());
    fs::copy(Path::new(config["qemu"]["ovmf_code"].as_str().unwrap()).to_path_buf(), dist_dir().join("ovmf_code.fd"))?;
    fs::copy(Path::new(config["qemu"]["ovmf_vars"].as_str().unwrap()).to_path_buf(), dist_dir().join("ovmf_vars.fd"))?;
    
    let status = Command::new(qemu.as_str().unwrap())
        .current_dir(project_root())
        .args(&["-drive", "if=pflash,format=raw,readonly=on,file=target/dist/ovmf_code.fd", "-drive", "if=pflash,format=raw,readonly=on,file=target/dist/ovmf_vars.fd", "-drive", "format=raw,file=fat:rw:target/dist/esp"])
        .status()?;
    if !status.success() {
        Err("run: qemu command failed")?;
    }

    Ok(())
}

fn dist() -> Result<(), Box<dyn std::error::Error>> {
    kernel()?;
    loader()?;
println!("creating");

    let _ = fs::remove_dir_all(&dist_dir());
    fs::create_dir_all(&dist_dir())?;
    fs::create_dir_all(&dist_dir().join("esp/efi/boot"))?;
    fs::copy(kernel_dir().join("hydra"), dist_dir().join("esp/efi/boot/hydra"))?;
    fs::copy(loader_dir().join("loader.efi"), dist_dir().join("esp/efi/boot/bootx64.efi"))?;

    Ok(())
}

fn kernel() -> Result<(), Box<dyn std::error::Error>> {
    let config = config();
    let cargo = env::var("CARGO").unwrap_or_else(|_| "cargo".to_string());
    let status = Command::new(cargo)
        .current_dir(project_root().join("sys"))
        .args(&["rustc", "--", "-C", &format!("link-arg=-T{}", config["sys"]["linker_script"].as_str().unwrap())])
        .status()?;
    if !status.success() {
        Err("kernel: cargo build failed")?;
    }

    Ok(())
}

fn loader() -> Result<(), Box<dyn std::error::Error>> {
    let cargo = env::var("CARGO").unwrap_or_else(|_| "cargo".to_string());
    let status = Command::new(cargo)
        .current_dir(project_root().join("loader"))
        .args(&["build"])
        .status()?;
    if !status.success() {
        Err("loader: cargo build failed")?;
    }

    Ok(())

}

fn project_root() -> PathBuf {
    Path::new(&env!("CARGO_MANIFEST_DIR"))
        .ancestors()
        .nth(1)
        .unwrap()
        .to_path_buf()
}

fn print_help() {
    eprintln!(
        "Tasks:

kernel      build kernel binary
"
    )
}

fn kernel_dir() -> PathBuf {
    project_root().join("target/x86_64-unknown-none/debug")
}

fn loader_dir() -> PathBuf {
    project_root().join("target/x86_64-unknown-uefi/debug")
}

fn dist_dir() -> PathBuf {
    project_root().join("target/dist")
}

fn config() -> Table {
    let data = fs::read_to_string("config.toml").unwrap();

    toml::from_str(&data).unwrap()
}
