# sfo2xml
sfo2xml is a converter for Sony's SFO format. It can convert it to XML and vice versa.

WARNING: This program supports only *game* SFO's, not *save data* SFO's. You can convert them, 
but the output won't match with the original SFO.

## Usage
Converting a SFO file to XML: 

```
sfo2xml.exe <input SFO file path> [output XML file path]
```

Converting an XML file to SFO: 

```
sfo2xml.exe <input XML file path> [output SFO file path]
```

## Building
1. Clone the repo

```
git clone https://github.com/TheExentist151/sfo2xml.git --recursive
```

2. Open the solution (`Source/sfo2xml.sln`) in Visual Studio 2022
3. Choose your configuration
4. Press Ctrl+B or click **Build** - **Build sfo2xml**

## External Resources
 * [PARAM.SFO documentation on psdevwiki.com](https://www.psdevwiki.com/ps3/PARAM.SFO)