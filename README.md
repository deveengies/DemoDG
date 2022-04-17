DemoDG

Utilitaire (client + serveur) d’échange de fichier synchronisé :
•    attendre la connexion de N utilisateurs ;
•    authentifier chaque utilisateur à partir d'une clé ;
•    lorsque tous les utilisateurs sont connectés, envoie d’un fichier prédéfini de manière sécurisée.


Ce module est une adaptation des exemples fournis pour la mise en oeuvres d'un service
de type 'WebSocket' de la bibliothèque C++ Boost (module 'Beast'), précisement en mode sécurisé (SSL) synchrone (écoute bloquante).

https://www.boost.org/doc/libs/1_79_0/libs/beast/doc/html/beast/examples.html

Server: websocket_server_sync_ssl.cpp
Client: websocket_client_sync_ssl.cpp

les modifications du Client se sont limités à l'ajout de la sauvegarde de la reception dans un fichier.
Celles du Serveur sont plus conséquentes (liste de clients prédéfinie, envoi de fichier, attente ...) mais la base protocolaire reste la même ...

Pour respecter, tant se peut, le CopyRight le depot GitHub 'DemoDG' est temporairement publique et sera privé à l'issue de la démo ...

L'authentification du client:
Lors de la connection,le client fournit son identifiant et une chaine de caractère correspondant à la première partie d'une chaine de caractère representant la clef d'encryptage /decriptage d'un fichier nommé avec son identifiant (extension'.cry') et contenant ce même identifiant (crypté donc).
le serveur possède la seconde partie de la clef; Il associe les deux parties, decode le fichier et vérifie que le fichier originel contient bien l'identifiant du client. 

 
 ##############################

Installation généralités

Les exécutables client/serveur sont à générer à partir du dépôt de sources GitHub :
https://github.com/deveengies/DemoDG
· Le dépôt est le même pour Linux et Windows
· Déployer dans un répertoire de votre choix (<DirDemoDG> contenant 'DemoDG' ) 
Les bibliothèques nécessaires sont :
· OpenSSL version 1.1 pour assurer la sécurité des transactions (réseaux) https://www.openssl.org/
· Boost  (bibliothèque c++) qui fournit, en particulier, des modules de développement réseaux (Asio, Beast)

· ##############################

Installation sur Linux

· Déployer DemoDG depuis GitHub dans le répertoire <DirDemoDG>
· Si necessaire installer libssl-dev, g++ et cmake
	o sudo apt install libssl-dev
	o sudo apt install g++
	o sudo apt install cmake
· Installation de OpenSSL si ce n’est pas probablement fait lors de l’installation système (Vérification ubuntu :  « apt list openssl »)
	o sudo apt install openssl-devel
· Installation de Boost - le repertoire de la librairie devra être placé dans <DirDemoDG> - 2 solutions:
	1) Depuis un  depot google correspondant à l'aboutissement de l'installation de Boost sur linux Ubuntu x64
		§ https://drive.google.com/file/d/1qVRXgcwE4xvLeC_alY21DihZ8xPoe2-k/view?usp=sharing 
		ce fichier '.tgz' contient le répertoire 'libraries' à placer dans <DemoDG>
	2) Suivre la procedure normale d'installation https://www.boost.org/users/download/
		§ Download 
		§  deploiement dans <DirDemoDG> 
		§  cd <DirDemoDG>/boost_1_<subversion>_0 
		§ ./bootstrap.bat
		§ ./b2
· Génération des binaires  - depuis un terminal et à partir du répertoire <DirDemoDG>/DemoDG 
	• cd  <DirDemoDG>/DemoDG
	• Vérifier et, éventuellement, corriger la cohérence des versions/directories de OpenSSL et de Boost dans les fichiers CMakeLists.txt des répertoires Client et Server
	• cmake .
	• make -j2 (les binaires Client et Serveur devraient être dans leur répertoires respectifs)


##############################

Installation sur Windows

· Si necessaire, installer 'visual studio communauté'  ( package C++)
	• https://visualstudio.microsoft.com/fr/downloads/
· Déployer DemoDG depuis GitHub dans le répertoire choisi (  <DirDemoDG> )
· Installation de OpenSSL si nécessaire le répertoire 'DirDemoDG' 
	• Necessité d'installer perl (https://strawberryperl.com/) (perl cygwin64 est en restriction)
	• Récuperer les sources
		§ https://www.openssl.org/source/mirror.html
			§ http://artfiles.org/openssl.org/ ( "mon choix")
				□ -> sources
					® openssl-3.0.2.tar.gz 
	• Dezipper openssl-3.0.2.tar.gz dans un répertoire temporaire (pour ce qui suit 'c:\tmp')
	•  ouvrir un cmdtools "prompt for VS 2022"
	• cd "le repertoire OpenSSL"d'installation dezippé (c:\tmp\openssl-master)
	• Lancer : perl Configure VC-WIN32 --prefix=<DirDemoDG>\OpenSSL --openssldir=<DirDemoDG>\OpenSSL\CommonFiles\SSL
	• Lancer le compilateur linkeur de visual studio: "nmake" ( … "where nmake" s'il n'est pas dans le PATH ) 
	
· Installation de Boost - le répertoire de la libraire devra être placé également dans <DirDemoDG> - 2 solutions:
	1) Depuis un  depot google correspondant à l'aboutissement de l'installation de Boost sur windows:
		§ https://drive.google.com/file/d/1UjLfcWFww1AtSGWT5EuvTp9mZAlhtuiC/view?usp=sharing
		ce fichier '.rar' contient le répertoire 'libraries' à placer dans <DemoDG>
	2) Suivre la procedure normale d'installation https://www.boost.org/users/download/
		§ Download , dézipper dans <DirDemoDG>
		§ cd <DirDemoDG>\boost_1_<subversion>_0 
		§ ./bootstrap.bat
		§ ./b2  --build-dir=build\x86 address-model=32 threading=multi --stagedir=.\bin\x86 --toolset=msvc -j 16 link=static,shared runtime-link=static,shared --variant=debug,release

. Génération des binaires
	. Ouvrir les deux projets Client/client.sln et Server/Server.sln separement.
	. Si necessaire configurer les plateformes en 'Debug/x86'
	. Lancer les générations (Ctrl+Maj+B)

##############################

Mise en œuvre et tests

Le test consiste à appliquer le scénario demandée:
	• Connection de 4 clients au serveur ('workDir\liste_clients.txt')
	• À l'issue, le serveur transmet un fichier prédéterminé aux clients ( 'workDir\toSend.txt')
	• Les clients sauvegardent ce fichier ('workDir\<client>_out.txt>

MeO Windows
    Le fichier <DirDemoDG>\scripts\Readme.test.windows décrit précisement la procédure.
	• Depuis une CmdTool: cd <DirDemoDG>\DemoDG\scripts
	• testFull
	• Verifier la présence des 4 fichiers <client>_out.txt


MeO Linux
    Le fichier <DirDemoDG>\scripts\Readme.test.linux décrit précisement la procédure.