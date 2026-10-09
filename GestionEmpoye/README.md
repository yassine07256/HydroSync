# HydroSync — Gestion des Employés (C++ / Qt 6 Widgets, qmake)

## Structure
```
HydroSync/
├── HydroSync.pro            projet qmake (ouvrir celui-ci dans Qt Creator)
├── main.cpp                 point d'entrée : charge style.qss, ouvre SQLite, affiche MainWindow
├── mainwindow.h / .cpp      menu latéral + barre bleue + QStackedWidget
├── homepage.*               page Accueil
├── employeepage.*           page Employé (tableau, filtres, stats, CRUD, PDF)
├── employeedialog.*         formulaire ajout / modification
├── employee.* database.*    modèle métier + SQLite
├── employeemodel.*  employeefilterproxymodel.*   données du tableau, filtres, tri
├── actiondelegate.*         icônes crayon / corbeille
├── pdfexporter.*            export PDF
├── charts.*                 donut, barres, anneaux
├── glasspanel.*             panneau blanc semi-transparent
├── backgroundwidget.*       dessine background.png
├── resources.qrc
└── resources/
    ├── style.qss            TOUT le design (couleurs, boutons, tableau)
    └── images/              background.png, hydrosync_logo.png, icônes .svg
```

## Compiler (Windows, Qt Creator)
1. Installez Qt 6.5+ avec le kit **MinGW 64-bit** (composants Qt Svg et Qt SQL inclus par défaut).
2. Extrayez le ZIP dans un chemin simple, sans accent (ex. `C:\Projets\HydroSync`).
3. Qt Creator > *Fichier > Ouvrir un fichier ou un projet* > **HydroSync.pro**.
4. Cochez le kit *Desktop Qt 6.x MinGW 64-bit* > *Configure Project*.
5. `Ctrl+B` pour compiler, `Ctrl+R` pour lancer.
(Si vous modifiez `style.qss` ou une image : *Build > Run qmake* puis recompilez, car ils sont embarqués par `resources.qrc`.)

## Image de fond
Remplacez `resources/images/background.png` par votre image (même nom). L'image fournie est un substitut.

## Données
SQLite : `%LOCALAPPDATA%\HydroSync\HydroSync\employees.sqlite` (créée au premier lancement avec 7 employés de démonstration ; supprimez le fichier pour réinitialiser).

## Modifier le design
* Couleurs / polices / boutons : `resources/style.qss` (sélecteurs par nom d'objet : `#topBar`, `#sidebar`, `#logoutButton`, `#employeeTable`, `#crudButton`, `#exportPdfButton`).
* Transparence d'un panneau : 2ᵉ paramètre de `new GlassPanel(nullptr, 215)` (0 = invisible, 255 = opaque).
* Icônes : remplacez les `.svg` de `resources/images/` (mêmes noms).

## Limitations
* « Départs ce mois » = valeur de démonstration (1) ; chiffres de l'accueil statiques.
* L'ID n'est pas modifiable lors d'une édition ; « déconnexion » ferme l'application après confirmation (pas d'écran de login).
