Instructions for Running SDL3 Applications on Xcode

1. Obtaining Resurces: (SDL3, SDL3_ttf, SDL3_image)

collect the following in your project directory:

Download the SDL3 Project from Github:
https://github.com/libsdl-org/SDL

And the SDL3 Xcode Project (.dmg file)
https://github.com/libsdl-org/SDL/releases

Download SDL3 TTF from Github:
https://github.com/libsdl-org/SDL_ttf

And the SDL3 TTF Xcode Project (.dmg file)
https://github.com/libsdl-org/SDL_ttf/releases

Downoad SDL3 Image from Github:
https://github.com/libsdl-org/SDL_image

And the SDL3 Image Cxode Project:
https://github.com/libsdl-org/SDL_image/releases


for each .dmg file Extract the .xcframework in to your project directory.



2. Create the Xcode Project:

Open Xcode and Create a ne Project.

Use the Game Template.

Name the Project and (for team) sing in your apple acount.

Organization Identifyer can be anything you want (your name or organization name is fine).

For Game Technology select Metal 4.

For Languige select Objective-C.

Select the desiured targed Platforms (standerd would be just macOS),
it is recomended to have only one platfor selected but selecting multiple can also work.
If you want to develop for iOS (Ipad or Iphone) select that platform now.

Select your project directory as the Project location, and Create the Project. (you can also select a difrent location if you want)

(you do not need github)



3. Prepare the Template:

Remove all the .h and .m files that got added to the project by default.

Remove the main storyboard that was automaticaly added to the project.

(Also remove shaders.metal)

!! Do not Remove Assets.xcassets as this is whre rhe app icon may be stored


3.5 (for IOS (Ipad, Iphone)):

Select the projet, under Targeds select the main targed, select the info tab, look for "Custom iOS Targed Properties",
and remove "Main storyboard base file name" and "Applications Scene Manifest".


4. Add your code to the Project:

Select the project, Rightcklick the Project and select "Add files to [project]", and add your project files.
(and anny assets you need)

(recomended action: Copy files to destination; Groups: Create Groups)



5. Add the SDL Xcode Projects to your Project:

Select the project, Rightcklick the Project and select "Add files to [project]", navigate to the SDL Xcode/SDL directory and add SDL.xcodeproj (Action: Reference files in place).

Select the project, Rightcklick the Project and select "Add files to [project]", navigate to the SDL_image Xcode directory (SDL_image-main/Xcode/) and add SDL_image.xcodeproj (Action: Reference files in place).

Select the project, Rightcklick the Project and select "Add files to [project]", navigate to the SDL_ttf Xcode directory (SDL_ttf-main/Xcode/) and add SDL_ttf.xcodeproj (Action: Reference files in place).


6. Add the SDL Xcode Frameworks:

Select the project, select the main targed, select the General Tab, look for "Frameworks, Libuarys, and Embedded Content", and add the following frameworks that you previusly extracted from the .dmg files (by drag and dropping):

SDL3.xcframework, SDL3_ttf.xcframework, and SDL3_image.xcframework.

Select Embedd and Sign for all of them.

7. Build and Runn by pressing the play button


Avoiding Errors and Problems:

Xcode for some reason sometimes complains if things may exsist twice (dublicate objects error) if things are done with extern.

You may need to comment out standerd libuary includes and then uncoment them so that x code stops complaining.

Accessing Assets and images:
All assets (like images and fonts) when the app is created in to the apps directory
to acces the assets you must create a path to these images using the SDL_GetBasePath() functon.

An example on how you can do this:

char image_path_buffer[500];
snprintf(image_path_buffer, 500, "%s%s", SDL_GetBasePath(), "image.jpg");

where "image.jpg" is the relative path to the image ass is in Xcode.
(in the example the image is just in the xcode project)


General File IO:

There are functions to answer necessary path questions:

    Where is my app's data? SDL_GetBasePath().
    Where can I safely write files? SDL_GetPrefPath().
    Where are paths like Downloads, Desktop, Music? SDL_GetUserFolder().
    What is this thing at this location? SDL_GetPathInfo().
    What items live in this folder? SDL_EnumerateDirectory().
    What items live in this folder by wildcard? SDL_GlobDirectory().
    What is my current working directory? SDL_GetCurrentDirectory().

When dealing with actualy opening and reading/writing to files it is recomended to use SDL functions
or use an abstraction as SDL functionsare in some ways similar to std file functions


Deploying to Physical device iOS:

1. Testing on a simulator:

This is optional but recomended to see if your App can even runn without major problems.
To test on the simulator you simply need to select your testdevice type at the very top in the middle.
The default test device will be the first destination when you select your project and look in the general tab under suported destinations.

To start the simulator press the playbutton (build and Runn) near the top left, this will start the simulator.
Note that the simulator may take a minut to boot / start.

2. Connecting a physical device:

Connect your Physical testing device to your mac via a cable and when the devices ask for trust, select trust.

3. Deploying to a physical device:

At the very to select your testing device as the targed.
(wether or not your device is listed on a targed may dependent on wether its apple acount is part of the developer team, and wether or not developer mode is enabled on the testing device
 see how to endable developer mode below)

Actvating Developer Mode:

if this is the first time you are doning this you need to Enable the Developer mode no your testing device,
to do this you need (on your testing device) go to settings "Privacy&securety", look for "developer mode" and endable it (your testing device must reboot to enable developer mode).


Build and runn using the play button, this will install the app on the testing device.
if a conection is astablished the application can also be installed over wifi.

4. Trusting the Test-App so it can be started:

To start the Application on your Testing device you must trust the application / the developer team in the settings.

To enable trust on your Testing device: on your Testing device go to settings "General", "VPN & Device managment", under "Developer-App" select your App and select trust and enable trust (trust: ...).



Seting the App Icon:

To se the app icon you need your icon as an 1024x1024 image (.png works fine .jpg shold also work) added to your Xcode project (selct your project, rightclick your project, select "add files to <project name>" and add your app icon image (action: copy files to destination)).
In your Xcode project under "<projectname> Shared" select the Assets catalog Typicaly named something like "Assets.xcassets",
there select app icon and drag and drop your App icon image in to the acording image squares.
To apply the changes you need to reinstall the app on your testing device.
