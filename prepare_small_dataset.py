from pathlib import Path
import shutil
from PIL import Image, UnidentifiedImageError


SOURCE_DIR = Path(r"C:\Users\Korisnik\Desktop\PetImages")
DEST_DIR = Path("dataset")

TRAIN_COUNT = 100
VALIDATION_COUNT = 25


def clear_destination():
    if DEST_DIR.exists():
        shutil.rmtree(DEST_DIR)

    print("Stari dataset folder je obrisan.")


def get_image_files(source_class_dir):
    return sorted([
        file for file in source_class_dir.iterdir()
        if file.suffix.lower() in [".jpg", ".jpeg", ".png"]
    ])


def save_clean_image(source_file, destination_file):
    """
    Otvara sliku pomoću Pillow biblioteke, pretvara ju u RGB
    i sprema kao novi ispravni JPG.
    Ako slika nije ispravna, vraća False.
    """
    try:
        with Image.open(source_file) as image:
            image = image.convert("RGB")
            image.save(destination_file, format="JPEG", quality=95)
        return True
    except (UnidentifiedImageError, OSError, ValueError) as error:
        print(f"Preskačem neispravnu sliku: {source_file} ({error})")
        return False


def copy_clean_images(source_class_dir, train_dest_dir, validation_dest_dir):
    image_files = get_image_files(source_class_dir)

    train_dest_dir.mkdir(parents=True, exist_ok=True)
    validation_dest_dir.mkdir(parents=True, exist_ok=True)

    train_copied = 0
    validation_copied = 0

    for file in image_files:
        if train_copied < TRAIN_COUNT:
            destination_file = train_dest_dir / f"{source_class_dir.name.lower()}_{train_copied}.jpg"

            if save_clean_image(file, destination_file):
                train_copied += 1

        elif validation_copied < VALIDATION_COUNT:
            destination_file = validation_dest_dir / f"{source_class_dir.name.lower()}_{validation_copied}.jpg"

            if save_clean_image(file, destination_file):
                validation_copied += 1

        else:
            break

    if train_copied < TRAIN_COUNT or validation_copied < VALIDATION_COUNT:
        raise ValueError(
            f"Nije pronađeno dovoljno ispravnih slika u {source_class_dir}. "
            f"Train: {train_copied}/{TRAIN_COUNT}, "
            f"Validation: {validation_copied}/{VALIDATION_COUNT}"
        )

    print(
        f"{source_class_dir.name}: kopirano "
        f"{train_copied} train i {validation_copied} validation slika."
    )


def main():
    clear_destination()

    copy_clean_images(
        SOURCE_DIR / "Cat",
        DEST_DIR / "train" / "cats",
        DEST_DIR / "validation" / "cats"
    )

    copy_clean_images(
        SOURCE_DIR / "Dog",
        DEST_DIR / "train" / "dogs",
        DEST_DIR / "validation" / "dogs"
    )

    print("Dataset je pripremljen.")


if __name__ == "__main__":
    main()