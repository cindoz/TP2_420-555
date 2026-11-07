# Bill of Materials (BoM) — Projet TP2 Dragula — Cindy

Pour le code :

https://github.com/cindoz/TP2_420-555

git clone https://github.com/cindoz/TP2_420-555.git

## 1. Tableau récapitulatif du matériel (BoM)

| Réf. Schéma | Désignation / Composant | Spécifications / Réf. | Boîtier / Format | Qté | Tension alim. | Broches / Connexions | Rôle dans le projet Dragula |
|---|---|---|---|:---:|:---:|---|---|
| **U1** | Microcontrôleur SoC | ESP32-WROOM-32 / DevKit v1 (**variante 30 broches**) | Module DevKit 30 broches | 1 | 3,3 V interne / 5 V USB | D4, D12, D13, D18, D19, D21, D22, D25, D26, D27, GND, 3V3 | Cerveau du système : lecture I2C, PWM moteur et servo, commande feux/klaxon/clignotant |
| **U2** | Convertisseur A/D I2C 8 canaux | ADS7830 (Module Freenove v2.2, adresse `0x4B`) | Breakout 16 broches (TSSOP-16) | 1 | +3,3 V | I2C (SDA=D21, SCL=D22), CH0=VRY, CH2=VRX, CH4=LDR | Acquisition analogique du joystick (X, Y) et de la photorésistance |
| **U3** | Driver de puissance | L293D (**Double pont en H**, 4 demi-ponts) | DIP-16 | 1 | +5 V (ext) | IN3=D13, IN4=D12, EN3,4=+5V, VCC1/VCC2=+5V, OUT3/OUT4=M1, GND=4,5,12,13; Entrées 1A, 2A, EN1,2 fixées au GND | Commande de puissance bidirectionnelle et vitesse PWM du moteur DC |
| **M1** | Moteur à courant continu (DC) | Moteur DC 5V/6V standard | 2 bornes filaires | 1 | +5 V (via L293D) | Connecté sur OUT3 (pin 11) et OUT4 (pin 14) du L293D | Propulsion (accélération, recul, vitesse proportionnelle de -50 à +100) |
| **M2** | Micro servomoteur | TowerPro SG90 (9g) | Fiche JR 3 broches | 1 | +5 V (ext) | Rouge=+5V ext, Brun/Noir=GND, Orange/Jaune=PWM (**GPIO 25 exclusif**) | Direction des roues (0° = gauche, 90° = neutre, 180° = droite) |
| **LCD** | Écran LCD avec sac à dos I2C | LCD 16×2 avec contrôleur PCF8574 (adresse `0x27`) | Module 4 broches | 1 | +5 V (ext) | VCC=+5V ext, GND=GND, SDA=D21, SCL=D22 | Affichage de la vitesse positive, flèche de direction et voyant clignotant |
| **JOY** | Module Joystick 2 axes + poussoir | Dual Axis Thumb Joystick | Module 5 broches | 1 | +3,3 V | VCC=+3,3V, GND=GND, VRX=ADS7830 CH2, VRY=ADS7830 CH0, SW=GPIO 4 | Commande d'accélération (axe Y), direction (axe X) et klaxon (SW) |
| **SW1** | Interrupteur indépendant | SW_SPDT (glissière / bascule) | THT 3 broches | 1 | Logique 3,3 V | Broche centrale = GPIO 18, Une borne ext. = GND, 3e borne = Non connectée (isolée) | Commande ON/OFF du clignotant (actif niveau bas via INPUT_PULLUP) |
| **D1** | DEL Clignotant | DEL Jaune 5 mm | Radial THT 5 mm | 1 | 3,3 V via R1 | **Anode sur R1 (reliée à GPIO 26)**, **Cathode au GND** | Voyant lumineux du clignotant (cycle 250 ms OFF / 500 ms ON) |
| **D2** | DEL Phare automatique | DEL Blanche (ou Rouge) 5 mm | Radial THT 5 mm | 1 | 3,3 V via R2 | **Anode sur R2 (reliée à GPIO 27)**, **Cathode au GND** | Éclairage automatique activé en obscurité (avec hystérésis) |
| **R_LDR** | Photorésistance | LDR 5 mm (GL5528) | Radial 2 broches | 1 | +3,3 V | Une borne à +3,3V, l'autre au point milieu (ADS7830 CH4) et R4 | Mesure de la luminosité ambiante (obscurité / clarté) |
| **BZ1** | Buzzer passif | Buzzer passif électromagnétique 5V | Cylindre 2 broches | 1 | +5 V (ext) | Borne (+) au +5V ext, Borne (-) au collecteur de Q1 | Klaxon sonore du véhicule commandé en fréquence/tonalité |
| **Q1** | Transistor bipolaire NPN | 2N2222 (ou 2N3904 / S8050) | TO-92 (Brochage E-B-C) | 1 | Commutation 5V | Base sur R3 (1 kΩ), Émetteur au GND, Collecteur au (-) du buzzer | Commutation de puissance pour le buzzer sans surcharger l'ESP32 |
| **R1** | Résistance série DEL | 220 Ω (1/4 W, 5%) | Axial DIN0207 | 1 | - | En série avec DEL clignotant D1 (limitation ~7 mA) sur GPIO 26 | Protection de la DEL D1 et du GPIO 26 |
| **R2** | Résistance série DEL | 220 Ω (1/4 W, 5%) | Axial DIN0207 | 1 | - | En série avec DEL phare D2 (limitation ~7 mA) sur GPIO 27 | Protection de la DEL D2 et du GPIO 27 |
| **R3** | Résistance de base | 1 kΩ (1/4 W, 5%) | Axial DIN0207 | 1 | - | Entre GPIO 19 et la base du transistor Q1 (~2,6 mA) | Courant de saturation de la base de Q1 |
| **R4** | Résistance fixe diviseur | 10 kΩ (1/4 W, 5%) | Axial DIN0207 | 1 | - | Entre point milieu (ADS7830 CH4) et GND | Complète le diviseur de tension avec la LDR sur +3,3V |
| **R41** | Résistance de tirage I2C | 10 kΩ (1/4 W, 5%) | Axial DIN0207 | 1 | +3,3 V | Pull-up entre ligne I2C SCL (GPIO 22) et +3,3V | Assure le bon niveau logique du bus I2C |
| **R42** | Résistance de tirage I2C | 10 kΩ (1/4 W, 5%) | Axial DIN0207 | 1 | +3,3 V | Pull-up entre ligne I2C SDA (GPIO 21) et +3,3V | Assure le bon niveau logique du bus I2C |
| **BT1 / ALIM** | Module alimentation breadboard | MB-102 / HW-131 + pile 9V (ou bloc DC 9V) | Module enfichable | 1 | Entrée 9V / Sortie +5V (1A max) | Sortie +5 V vers rails de puissance, GND au GND commun | Alimentation propre des éléments énergivores (moteur, servo, LCD, buzzer) |
| **BB1 / BB2** | Platines d'expérimentation | 2 breadboards 830 points (BG et BD) | - | 2 | - | Organisation BG (gauche) et BD (droite) avec pontage des rails | Support physique d'interconnexion du prototype |
| **CÂBLES** | Fils de raccordement | Câbles Dupont (M-M, M-F, F-F) | Assortiment | ~30 | - | Interconnexions | Câblage |

---

## 2. Table d'affectation des GPIOs de l'ESP32

| GPIO ESP32 | Mode ESP32 | Composant relié | Description fonctionnelle |
|:---:|:---:|---|---|
| **GPIO 4** | `INPUT_PULLUP` | Bouton SW du joystick | Détection appui joystick (active le klaxon) |
| **GPIO 12** | `OUTPUT` | L293D IN4 (broche 15) | Commande recul / PWM moteur DC *(débrancher au téléversement)* |
| **GPIO 13** | `OUTPUT` | L293D IN3 (broche 10) | Commande avance / PWM moteur DC |
| **GPIO 18** | `INPUT_PULLUP` | Interrupteur SW1 (indépendant) | Activation / désactivation du clignotant |
| **GPIO 19** | `OUTPUT` (PWM / tone) | Base Q1 (2N2222) via R3 (1 kΩ) | Commande audio du klaxon (buzzer passif) |
| **GPIO 21** | `OUTPUT / INPUT` (I2C) | Ligne I2C SDA | Données bus I2C partagé (ADS7830 + LCD) |
| **GPIO 22** | `OUTPUT` (I2C) | Ligne I2C SCL | Horloge bus I2C partagé (ADS7830 + LCD) |
| **GPIO 25** | `OUTPUT` (Servo) | Signal orange Servomoteur SG90 | Contrôle angulaire de direction (0° à 180°) |
| **GPIO 26** | `OUTPUT` | Anode DEL D1 via R1 (220 Ω) | Clignotement de la lumière clignotant |
| **GPIO 27** | `OUTPUT` | Anode DEL D2 via R2 (220 Ω) | Éclairage automatique des phares de nuit |

---

## 3. Table des canaux ADC ADS7830 (Adresse I2C : 0x4B)

| Canal ADS7830 | Broche physique | Composant relié | Plage de tension | Grandeur mesurée |
|:---:|:---:|---|:---:|---|
| **CH0 (A0)** | Broche 1 | Potentiomètre Joystick VRY (Axe Y) | 0 V à 3,3 V | Accélération avant / arrière du véhicule |
| **CH2 (A2)** | Broche 3 | Potentiomètre Joystick VRX (Axe X) | 0 V à 3,3 V | Angle de braquage (orientation servo 0°–180°) |
| **CH4 (A4)** | Broche 5 | Point milieu diviseur LDR + 10 kΩ | 0 V à 3,3 V | Éclairement ambiant pour allumage automatique |

---
