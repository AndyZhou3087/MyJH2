<GameFile>
  <PropertyGroup Name="libraryLayer" Type="Layer" ID="0380b204-fdf3-44d3-b504-531aaff25065" Version="3.10.0.0" />
  <Content ctype="GameProjectContent">
    <Content>
      <Animation Duration="0" Speed="1.0000" />
      <ObjectData Name="Layer" ctype="GameLayerObjectData">
        <Size X="720.0000" Y="1280.0000" />
        <Children>
          <AbstractNodeData Name="bg" ActionTag="2137255485" Tag="11" IconVisible="False" LeftEage="237" RightEage="237" TopEage="422" BottomEage="422" Scale9OriginX="237" Scale9OriginY="422" Scale9Width="246" Scale9Height="436" ctype="ImageViewObjectData">
            <Size X="720.0000" Y="1280.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="360.0000" Y="640.0000" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.5000" Y="0.5000" />
            <PreSize X="1.0000" Y="1.0000" />
            <FileData Type="Normal" Path="images/shopbg.jpg" Plist="" />
          </AbstractNodeData>
          <AbstractNodeData Name="librarytop" ActionTag="-1393793010" Tag="62" IconVisible="False" BottomMargin="1105.0000" ctype="SpriteObjectData">
            <Size X="720.0000" Y="175.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="1.0000" />
            <Position X="360.0000" Y="1280.0000" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.5000" Y="1.0000" />
            <PreSize X="1.0000" Y="0.1367" />
            <FileData Type="Normal" Path="images/librarytop.png" Plist="" />
            <BlendFunc Src="1" Dst="771" />
          </AbstractNodeData>
          <AbstractNodeData Name="buildingtitlebox" ActionTag="276143185" Tag="61" IconVisible="False" LeftMargin="247.9999" RightMargin="248.0001" TopMargin="74.0049" BottomMargin="1135.9951" ctype="SpriteObjectData">
            <Size X="224.0000" Y="70.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="359.9999" Y="1170.9951" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.5000" Y="0.9148" />
            <PreSize X="0.3111" Y="0.0547" />
            <FileData Type="MarkedSubImage" Path="ui/buildingtitlebox.png" Plist="ui.plist" />
            <BlendFunc Src="1" Dst="771" />
          </AbstractNodeData>
          <AbstractNodeData Name="title" ActionTag="1419317784" Tag="141" IconVisible="False" LeftMargin="311.5001" RightMargin="311.4999" TopMargin="87.5000" BottomMargin="1157.5000" LeftEage="25" RightEage="25" TopEage="12" BottomEage="12" Scale9OriginX="25" Scale9OriginY="12" Scale9Width="47" Scale9Height="11" ctype="ImageViewObjectData">
            <Size X="97.0000" Y="35.0000" />
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="360.0001" Y="1175.0000" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.5000" Y="0.9180" />
            <PreSize X="0.1347" Y="0.0273" />
            <FileData Type="MarkedSubImage" Path="text_zh_cn/librarytitle.png" Plist="text_zh_cn.plist" />
          </AbstractNodeData>
          <AbstractNodeData Name="backbtn" ActionTag="1173035810" Tag="63" IconVisible="False" LeftMargin="258.0000" RightMargin="258.0000" TopMargin="1164.8398" BottomMargin="31.1602" TouchEnable="True" FontSize="14" Scale9Enable="True" LeftEage="15" RightEage="15" TopEage="11" BottomEage="11" Scale9OriginX="15" Scale9OriginY="11" Scale9Width="174" Scale9Height="62" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="ButtonObjectData">
            <Size X="204.0000" Y="84.0000" />
            <Children>
              <AbstractNodeData Name="text" ActionTag="-484697184" Tag="64" IconVisible="False" LeftMargin="67.2415" RightMargin="62.7585" TopMargin="22.4025" BottomMargin="27.5975" LeftEage="24" RightEage="24" TopEage="13" BottomEage="13" Scale9OriginX="24" Scale9OriginY="13" Scale9Width="26" Scale9Height="8" ctype="ImageViewObjectData">
                <Size X="74.0000" Y="34.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="104.2415" Y="44.5975" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.5110" Y="0.5309" />
                <PreSize X="0.3627" Y="0.4048" />
                <FileData Type="MarkedSubImage" Path="text_zh_cn/backbtn_text.png" Plist="text_zh_cn.plist" />
              </AbstractNodeData>
            </Children>
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="360.0000" Y="73.1602" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.5000" Y="0.0572" />
            <PreSize X="0.2833" Y="0.0656" />
            <TextColor A="255" R="65" G="65" B="70" />
            <DisabledFileData Type="MarkedSubImage" Path="ui/buildingactbtn_d.png" Plist="ui.plist" />
            <PressedFileData Type="MarkedSubImage" Path="ui/buildingactbtn_n.png" Plist="ui.plist" />
            <NormalFileData Type="MarkedSubImage" Path="ui/buildingactbtn_n.png" Plist="ui.plist" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
          <AbstractNodeData Name="equipitem" ActionTag="-178317115" Tag="12" IconVisible="False" LeftMargin="27.0002" RightMargin="6.9998" TopMargin="178.9886" BottomMargin="823.0114" TouchEnable="True" LeftEage="226" RightEage="226" TopEage="91" BottomEage="91" Scale9OriginX="226" Scale9OriginY="91" Scale9Width="234" Scale9Height="96" ctype="ImageViewObjectData">
            <Size X="686.0000" Y="278.0000" />
            <Children>
              <AbstractNodeData Name="text" ActionTag="1529565062" Tag="13" IconVisible="False" LeftMargin="119.0000" RightMargin="325.0000" TopMargin="102.0000" BottomMargin="104.0000" LeftEage="80" RightEage="80" TopEage="24" BottomEage="24" Scale9OriginX="80" Scale9OriginY="24" Scale9Width="82" Scale9Height="24" ctype="ImageViewObjectData">
                <Size X="242.0000" Y="72.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="240.0000" Y="140.0000" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.3499" Y="0.5036" />
                <PreSize X="0.3528" Y="0.2590" />
                <FileData Type="MarkedSubImage" Path="text_zh_cn/libraryequiptext.png" Plist="text_zh_cn.plist" />
              </AbstractNodeData>
            </Children>
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="370.0002" Y="962.0114" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.5139" Y="0.7516" />
            <PreSize X="0.9528" Y="0.2172" />
            <FileData Type="Normal" Path="images/libraryequipitem.png" Plist="" />
          </AbstractNodeData>
          <AbstractNodeData Name="wgitem" ActionTag="1003016574" Tag="65" IconVisible="False" LeftMargin="17.0005" RightMargin="16.9995" TopMargin="530.9919" BottomMargin="471.0081" TouchEnable="True" LeftEage="226" RightEage="226" TopEage="91" BottomEage="91" Scale9OriginX="226" Scale9OriginY="91" Scale9Width="234" Scale9Height="96" ctype="ImageViewObjectData">
            <Size X="686.0000" Y="278.0000" />
            <Children>
              <AbstractNodeData Name="text" ActionTag="1198811915" Tag="139" IconVisible="False" LeftMargin="117.0000" RightMargin="323.0000" TopMargin="100.5000" BottomMargin="102.5000" LeftEage="81" RightEage="81" TopEage="24" BottomEage="24" Scale9OriginX="81" Scale9OriginY="24" Scale9Width="84" Scale9Height="27" ctype="ImageViewObjectData">
                <Size X="246.0000" Y="75.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="240.0000" Y="140.0000" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.3499" Y="0.5036" />
                <PreSize X="0.3586" Y="0.2698" />
                <FileData Type="MarkedSubImage" Path="text_zh_cn/librarywgtext.png" Plist="text_zh_cn.plist" />
              </AbstractNodeData>
            </Children>
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="360.0005" Y="610.0081" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.5000" Y="0.4766" />
            <PreSize X="0.9528" Y="0.2172" />
            <FileData Type="Normal" Path="images/librarywgitem.png" Plist="" />
          </AbstractNodeData>
          <AbstractNodeData Name="ngitem" ActionTag="-967399148" Tag="66" IconVisible="False" LeftMargin="17.0006" RightMargin="16.9994" TopMargin="882.9951" BottomMargin="119.0049" TouchEnable="True" LeftEage="226" RightEage="226" TopEage="91" BottomEage="91" Scale9OriginX="226" Scale9OriginY="91" Scale9Width="234" Scale9Height="96" ctype="ImageViewObjectData">
            <Size X="686.0000" Y="278.0000" />
            <Children>
              <AbstractNodeData Name="text" ActionTag="2116494189" Tag="140" IconVisible="False" LeftMargin="117.5000" RightMargin="323.5000" TopMargin="100.5000" BottomMargin="102.5000" LeftEage="80" RightEage="80" TopEage="24" BottomEage="24" Scale9OriginX="80" Scale9OriginY="24" Scale9Width="85" Scale9Height="27" ctype="ImageViewObjectData">
                <Size X="245.0000" Y="75.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="240.0000" Y="140.0000" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.3499" Y="0.5036" />
                <PreSize X="0.3571" Y="0.2698" />
                <FileData Type="MarkedSubImage" Path="text_zh_cn/libraryngtext.png" Plist="text_zh_cn.plist" />
              </AbstractNodeData>
            </Children>
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="360.0006" Y="258.0049" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.5000" Y="0.2016" />
            <PreSize X="0.9528" Y="0.2172" />
            <FileData Type="Normal" Path="images/libraryngitem.png" Plist="" />
          </AbstractNodeData>
        </Children>
      </ObjectData>
    </Content>
  </Content>
</GameFile>