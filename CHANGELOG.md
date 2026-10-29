# Changelog

## [v1.0.0] - 2026-10-29

### Novinky a funkce
- **Posun Odkazu dle GB:** Nový příkaz v menu `Hotline Cegra` pro Archicad 29 a Archicad 30.
- **Automatické zarovnání průhledového zobrazení:**
  - Zarovná referenci podle přesné pozice Geodetického bodu (Survey Point).
  - Nastaví rotaci podle úhlu Severu ($\theta = \text{northAngle} - \pi/2$).
  - Plně eliminuje fenomén přepočtu souřadnic daleko od počátku (Far From Origin) po uložení a znovuotevření projektu započtením virtuálních posunů obou oken (`parentOffset` a `ghostOffset`).
- **Nová vektorová ikona:** Dvouvrstvá SVG/TIFF ikona (`Posun_Pruhled_GB_18x18.svg`) s plnou podporou pro Light i Dark Mode.
- **Cross-platformní podpora:**
  - macOS: `.bundle` pro Archicad 29 a 30 podepsaný certifikátem Apple Developer ID.
  - Windows: `.apx` pro Archicad 29 a 30 sestavený pomocí MSVC 2022 přes GitHub Actions.
