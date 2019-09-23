<GameFile>
  <PropertyGroup Name="fightResultLayer" Type="Layer" ID="4d6d6350-9798-4d3d-9d3e-9c2572766946" Version="3.10.0.0" />
  <Content ctype="GameProjectContent">
    <Content>
      <Animation Duration="0" Speed="1.0000" />
      <ObjectData Name="Layer" Tag="53" ctype="GameLayerObjectData">
        <Size X="720.0000" Y="1280.0000" />
        <Children>
          <AbstractNodeData Name="actionbtn" ActionTag="466477617" Tag="88" IconVisible="False" LeftMargin="258.0000" RightMargin="258.0000" TopMargin="1168.0000" BottomMargin="28.0000" TouchEnable="True" FontSize="14" Scale9Enable="True" LeftEage="15" RightEage="15" TopEage="11" BottomEage="11" Scale9OriginX="15" Scale9OriginY="11" Scale9Width="174" Scale9Height="62" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="ButtonObjectData">
            <Size X="204.0000" Y="84.0000" />
            <Children>
              <AbstractNodeData Name="text" ActionTag="70498527" Tag="89" IconVisible="False" LeftMargin="61.5216" RightMargin="62.4784" TopMargin="19.7404" BottomMargin="24.2596" LeftEage="25" RightEage="25" TopEage="13" BottomEage="13" Scale9OriginX="25" Scale9OriginY="13" Scale9Width="30" Scale9Height="14" ctype="ImageViewObjectData">
                <Size X="80.0000" Y="40.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="101.5216" Y="44.2596" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition X="0.4977" Y="0.5269" />
                <PreSize X="0.3922" Y="0.4762" />
                <FileData Type="MarkedSubImage" Path="text_zh_cn/continuebtn_text.png" Plist="text_zh_cn.plist" />
              </AbstractNodeData>
            </Children>
            <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
            <Position X="360.0000" Y="70.0000" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.5000" Y="0.0547" />
            <PreSize X="0.2833" Y="0.0656" />
            <TextColor A="255" R="65" G="65" B="70" />
            <DisabledFileData Type="MarkedSubImage" Path="ui/buildingactbtn_d.png" Plist="ui.plist" />
            <PressedFileData Type="MarkedSubImage" Path="ui/buildingactbtn_n.png" Plist="ui.plist" />
            <NormalFileData Type="MarkedSubImage" Path="ui/buildingactbtn_n.png" Plist="ui.plist" />
            <OutlineColor A="255" R="255" G="0" B="0" />
            <ShadowColor A="255" R="110" G="110" B="110" />
          </AbstractNodeData>
          <AbstractNodeData Name="matchinfonode" ActionTag="1927496671" Tag="126" IconVisible="True" LeftMargin="360.0000" RightMargin="360.0000" TopMargin="1020.0000" BottomMargin="260.0000" ctype="SingleNodeObjectData">
            <Size X="0.0000" Y="0.0000" />
            <Children>
              <AbstractNodeData Name="fightretline_1" ActionTag="918966727" Tag="125" IconVisible="False" LeftMargin="-269.0000" RightMargin="91.0000" TopMargin="-14.1411" BottomMargin="-18.8589" ctype="SpriteObjectData">
                <Size X="178.0000" Y="33.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="-180.0000" Y="-2.3589" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FileData Type="MarkedSubImage" Path="ui/fightretline.png" Plist="ui.plist" />
                <BlendFunc Src="1" Dst="771" />
              </AbstractNodeData>
              <AbstractNodeData Name="getscoretext" ActionTag="-1946341882" Tag="158" IconVisible="False" LeftMargin="-79.0000" RightMargin="-79.0000" TopMargin="-19.5000" BottomMargin="-19.5000" LeftEage="52" RightEage="52" TopEage="12" BottomEage="12" Scale9OriginX="52" Scale9OriginY="12" Scale9Width="54" Scale9Height="15" ctype="ImageViewObjectData">
                <Size X="158.0000" Y="39.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FileData Type="MarkedSubImage" Path="text_zh_cn/getscore_text.png" Plist="text_zh_cn.plist" />
              </AbstractNodeData>
              <AbstractNodeData Name="fightretline_2" ActionTag="458235504" Tag="127" IconVisible="False" LeftMargin="91.0000" RightMargin="-269.0000" TopMargin="-14.1411" BottomMargin="-18.8589" FlipX="True" ctype="SpriteObjectData">
                <Size X="178.0000" Y="33.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="180.0000" Y="-2.3589" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FileData Type="MarkedSubImage" Path="ui/fightretline.png" Plist="ui.plist" />
                <BlendFunc Src="1" Dst="771" />
              </AbstractNodeData>
              <AbstractNodeData Name="winscoretext" ActionTag="505509070" Tag="154" IconVisible="False" LeftMargin="-84.8212" RightMargin="-32.1788" TopMargin="40.7779" BottomMargin="-72.7779" FontSize="26" LabelText="本局积分：" OutlineEnabled="True" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
                <Size X="117.0000" Y="32.0000" />
                <AnchorPoint ScaleY="0.6176" />
                <Position X="-84.8212" Y="-53.0147" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FontResource Type="Normal" Path="fonts/simhei.ttf" Plist="" />
                <OutlineColor A="255" R="0" G="0" B="0" />
                <ShadowColor A="255" R="110" G="110" B="110" />
              </AbstractNodeData>
              <AbstractNodeData Name="winscore" ActionTag="-1088770092" Tag="155" IconVisible="False" LeftMargin="42.3255" RightMargin="-84.3255" TopMargin="42.0133" BottomMargin="-74.0133" FontSize="26" LabelText="+30" OutlineEnabled="True" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
                <Size X="42.0000" Y="32.0000" />
                <AnchorPoint ScaleY="0.6176" />
                <Position X="42.3255" Y="-54.2501" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FontResource Type="Normal" Path="fonts/simhei.ttf" Plist="" />
                <OutlineColor A="255" R="0" G="0" B="0" />
                <ShadowColor A="255" R="110" G="110" B="110" />
              </AbstractNodeData>
              <AbstractNodeData Name="myscoretext" ActionTag="-784805180" Tag="156" IconVisible="False" LeftMargin="-84.8212" RightMargin="-32.1788" TopMargin="82.3122" BottomMargin="-114.3122" FontSize="26" LabelText="我的积分：" OutlineEnabled="True" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
                <Size X="117.0000" Y="32.0000" />
                <AnchorPoint ScaleY="0.6176" />
                <Position X="-84.8212" Y="-94.5490" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FontResource Type="Normal" Path="fonts/simhei.ttf" Plist="" />
                <OutlineColor A="255" R="0" G="0" B="0" />
                <ShadowColor A="255" R="110" G="110" B="110" />
              </AbstractNodeData>
              <AbstractNodeData Name="myscore" ActionTag="168954003" Tag="157" IconVisible="False" LeftMargin="42.3255" RightMargin="-71.3255" TopMargin="83.1333" BottomMargin="-115.1333" FontSize="26" LabelText="30" OutlineEnabled="True" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
                <Size X="29.0000" Y="32.0000" />
                <AnchorPoint ScaleY="0.6176" />
                <Position X="42.3255" Y="-95.3701" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FontResource Type="Normal" Path="fonts/simhei.ttf" Plist="" />
                <OutlineColor A="255" R="0" G="0" B="0" />
                <ShadowColor A="255" R="110" G="110" B="110" />
              </AbstractNodeData>
            </Children>
            <AnchorPoint />
            <Position X="360.0000" Y="260.0000" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.5000" Y="0.2031" />
            <PreSize X="0.0000" Y="0.0000" />
          </AbstractNodeData>
          <AbstractNodeData Name="continuenode" ActionTag="184541811" Tag="44" IconVisible="True" LeftMargin="360.0000" RightMargin="360.0000" TopMargin="1020.0000" BottomMargin="260.0000" ctype="SingleNodeObjectData">
            <Size X="0.0000" Y="0.0000" />
            <Children>
              <AbstractNodeData Name="fightretline_1" ActionTag="474424093" Tag="45" IconVisible="False" LeftMargin="-269.0000" RightMargin="91.0000" TopMargin="-14.1411" BottomMargin="-18.8589" ctype="SpriteObjectData">
                <Size X="178.0000" Y="33.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="-180.0000" Y="-2.3589" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FileData Type="MarkedSubImage" Path="ui/fightretline.png" Plist="ui.plist" />
                <BlendFunc Src="1" Dst="771" />
              </AbstractNodeData>
              <AbstractNodeData Name="getscoretext" ActionTag="-1781327544" Tag="46" IconVisible="False" LeftMargin="-79.0000" RightMargin="-79.0000" TopMargin="-19.5000" BottomMargin="-19.5000" LeftEage="52" RightEage="52" TopEage="12" BottomEage="12" Scale9OriginX="52" Scale9OriginY="12" Scale9Width="54" Scale9Height="15" ctype="ImageViewObjectData">
                <Size X="158.0000" Y="39.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FileData Type="MarkedSubImage" Path="text_zh_cn/continuewin_text.png" Plist="text_zh_cn.plist" />
              </AbstractNodeData>
              <AbstractNodeData Name="fightretline_2" ActionTag="1349674958" Tag="47" IconVisible="False" LeftMargin="91.0000" RightMargin="-269.0000" TopMargin="-14.1411" BottomMargin="-18.8589" FlipX="True" ctype="SpriteObjectData">
                <Size X="178.0000" Y="33.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="180.0000" Y="-2.3589" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FileData Type="MarkedSubImage" Path="ui/fightretline.png" Plist="ui.plist" />
                <BlendFunc Src="1" Dst="771" />
              </AbstractNodeData>
              <AbstractNodeData Name="continuewintext" ActionTag="1780414493" Tag="42" IconVisible="False" LeftMargin="-306.5000" RightMargin="-306.5000" TopMargin="69.0000" BottomMargin="-101.0000" FontSize="26" LabelText="当前已经连续胜利*场（连续胜利*场可以获得一颗星）" OutlineEnabled="True" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
                <Size X="613.0000" Y="32.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position Y="-85.0000" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FontResource Type="Normal" Path="fonts/simhei.ttf" Plist="" />
                <OutlineColor A="255" R="0" G="0" B="0" />
                <ShadowColor A="255" R="110" G="110" B="110" />
              </AbstractNodeData>
            </Children>
            <AnchorPoint />
            <Position X="360.0000" Y="260.0000" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.5000" Y="0.2031" />
            <PreSize X="0.0000" Y="0.0000" />
          </AbstractNodeData>
          <AbstractNodeData Name="failhintnode" ActionTag="-1071770961" Tag="52" IconVisible="True" LeftMargin="360.0000" RightMargin="360.0000" TopMargin="1020.0000" BottomMargin="260.0000" ctype="SingleNodeObjectData">
            <Size X="0.0000" Y="0.0000" />
            <Children>
              <AbstractNodeData Name="fightretline_1" ActionTag="3775813" Tag="53" IconVisible="False" LeftMargin="-269.0000" RightMargin="91.0000" TopMargin="-14.1411" BottomMargin="-18.8589" ctype="SpriteObjectData">
                <Size X="178.0000" Y="33.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="-180.0000" Y="-2.3589" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FileData Type="MarkedSubImage" Path="ui/fightretline.png" Plist="ui.plist" />
                <BlendFunc Src="1" Dst="771" />
              </AbstractNodeData>
              <AbstractNodeData Name="getscoretext" ActionTag="-934013146" Tag="54" IconVisible="False" LeftMargin="-79.0000" RightMargin="-79.0000" TopMargin="-19.5000" BottomMargin="-19.5000" LeftEage="52" RightEage="52" TopEage="12" BottomEage="12" Scale9OriginX="52" Scale9OriginY="12" Scale9Width="54" Scale9Height="15" ctype="ImageViewObjectData">
                <Size X="158.0000" Y="39.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FileData Type="MarkedSubImage" Path="text_zh_cn/getwin_text.png" Plist="text_zh_cn.plist" />
              </AbstractNodeData>
              <AbstractNodeData Name="fightretline_2" ActionTag="341893293" Tag="55" IconVisible="False" LeftMargin="91.0000" RightMargin="-269.0000" TopMargin="-14.1411" BottomMargin="-18.8589" FlipX="True" ctype="SpriteObjectData">
                <Size X="178.0000" Y="33.0000" />
                <AnchorPoint ScaleX="0.5000" ScaleY="0.5000" />
                <Position X="180.0000" Y="-2.3589" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FileData Type="MarkedSubImage" Path="ui/fightretline.png" Plist="ui.plist" />
                <BlendFunc Src="1" Dst="771" />
              </AbstractNodeData>
              <AbstractNodeData Name="hint1" ActionTag="984972982" Tag="56" IconVisible="False" LeftMargin="-185.4993" RightMargin="-185.5007" TopMargin="36.9998" BottomMargin="-64.9998" FontSize="23" LabelText="在地图神秘商店购买厉害的武器武功" OutlineEnabled="True" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
                <Size X="371.0000" Y="28.0000" />
                <AnchorPoint ScaleY="0.5000" />
                <Position X="-185.4993" Y="-50.9998" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FontResource Type="Normal" Path="fonts/simhei.ttf" Plist="" />
                <OutlineColor A="255" R="0" G="0" B="0" />
                <ShadowColor A="255" R="110" G="110" B="110" />
              </AbstractNodeData>
              <AbstractNodeData Name="hint2" ActionTag="1423119424" Tag="57" IconVisible="False" LeftMargin="-185.4993" RightMargin="-185.5007" TopMargin="75.9997" BottomMargin="-103.9997" FontSize="23" LabelText="在地图神秘商店购买厉害的武器武功" OutlineEnabled="True" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
                <Size X="371.0000" Y="28.0000" />
                <AnchorPoint ScaleY="0.5000" />
                <Position X="-185.4993" Y="-89.9997" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FontResource Type="Normal" Path="fonts/simhei.ttf" Plist="" />
                <OutlineColor A="255" R="0" G="0" B="0" />
                <ShadowColor A="255" R="110" G="110" B="110" />
              </AbstractNodeData>
              <AbstractNodeData Name="hint3" ActionTag="-545674143" Tag="58" IconVisible="False" LeftMargin="-185.4993" RightMargin="-185.5007" TopMargin="112.9994" BottomMargin="-140.9994" FontSize="23" LabelText="在地图神秘商店购买厉害的武器武功" OutlineEnabled="True" ShadowOffsetX="2.0000" ShadowOffsetY="-2.0000" ctype="TextObjectData">
                <Size X="371.0000" Y="28.0000" />
                <AnchorPoint ScaleY="0.5000" />
                <Position X="-185.4993" Y="-126.9994" />
                <Scale ScaleX="1.0000" ScaleY="1.0000" />
                <CColor A="255" R="255" G="255" B="255" />
                <PrePosition />
                <PreSize X="0.0000" Y="0.0000" />
                <FontResource Type="Normal" Path="fonts/simhei.ttf" Plist="" />
                <OutlineColor A="255" R="0" G="0" B="0" />
                <ShadowColor A="255" R="110" G="110" B="110" />
              </AbstractNodeData>
            </Children>
            <AnchorPoint />
            <Position X="360.0000" Y="260.0000" />
            <Scale ScaleX="1.0000" ScaleY="1.0000" />
            <CColor A="255" R="255" G="255" B="255" />
            <PrePosition X="0.5000" Y="0.2031" />
            <PreSize X="0.0000" Y="0.0000" />
          </AbstractNodeData>
        </Children>
      </ObjectData>
    </Content>
  </Content>
</GameFile>