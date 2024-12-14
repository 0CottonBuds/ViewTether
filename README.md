# ViewTether

ViewTether allows you to stream a virtual second screen from your Windows machine to another device, such as Android or desktop, effectively transforming it into a secondary display. To use this software, you'll need to download its companion [client application](https://github.com/0CottonBuds/ViewTether-Android-Client).

## Acknowledgements

This project includes an unmodified version of Amyuni USB Mobile Monitor Virtual Display, copyrighted by Amyuni Technologies Inc. For more details, please visit the [official Amyuni website](https://www.amyuni.com).

## License

ViewTether is licensed under the GPL v3 License. See LICENSE.md for details.

This project also includes FFmpeg, licensed under GPL. A copy of the GPL license is provided in the `FFmpeg-source` directory. For more information on FFmpeg and its licensing terms, visit the [official FFmpeg website](https://ffmpeg.org/legal.html).

The Amyuni USB Mobile Monitor Virtual Display software is licensed under terms provided by Amyuni Technologies Inc. Refer to the accompanying license file for specifics.

## Dependencies

This application relies on FFmpeg for compressing frames. Ensure you have it linked in your development environment.

## Development pains

As of not when building this project I have to manually copy paste the ffmpeg libraries to the build directory and after that run the windeployqt.exe to ViewTether.exe I will explore how I can automate this but for now I will focus on development.
