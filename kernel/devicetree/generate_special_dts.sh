#!/bin/bash

# Generate special DTS files for Kodi and GameScope builds
# This script creates specialized DTS files for media center and gaming builds

DEVICETREE_DIR="."
DISTROS=("debian-11" "debian-12" "debian-13" "ubuntu-22.04" "ubuntu-24.04" "ubuntu-25.04")
KERNELS=("6.8" "6.9" "6.10" "6.11" "6.12" "6.13" "6.14" "6.15.4")

# Function to create Kodi-optimized DTS
create_kodi_dts() {
    local distro=$1
    local kernel=$2
    
    cat << EOF
/dts-v1/;
#include <dt-bindings/gpio/gpio.h>
#include <dt-bindings/pinctrl/rockchip.h>
#include "rk3588s.dtsi"
#include "rk3588s-orangepi-5.dtsi"

/ {
    model = "Orange Pi 5 Plus - ${distro} ${kernel} - Kodi Optimized";
    compatible = "xunlong,orangepi-5-plus", "rockchip,rk3588s";
    
    metadata {
        build-type = "kodi-media-center";
        kernel-version = "${kernel}";
        distro-name = "$(echo ${distro} | cut -d'-' -f1)";
        distro-version = "$(echo ${distro} | cut -d'-' -f2)";
        optimization = "media-playback";
        features = "hdr10,av1-decode,hevc-10bit,cec";
    };

    chosen {
        bootargs = "console=ttyS2,1500000n8 root=/dev/mmcblk0p2 rootwait rw quiet loglevel=3 plymouth.enable=0";
    };

    reserved-memory {
        #address-cells = <2>;
        #size-cells = <2>;
        ranges;

        linux,cma {
            compatible = "shared-dma-pool";
            reusable;
            size = <0x0 0x40000000>; /* 1GB for 4K video */
            alignment = <0x0 0x400000>;
            linux,cma-default;
        };
    };
};

/* Video decoders */
&vdec {
    status = "okay";
    rockchip,h264-decoder;
    rockchip,h265-decoder;
    rockchip,vp9-decoder;
    rockchip,av1-decoder;
    rockchip,max-width = <3840>;
    rockchip,max-height = <2160>;
    rockchip,max-fps = <60>;
};

/* GPU for Kodi GUI */
&gpu {
    status = "okay";
    operating-points-v2 = <&gpu_kodi_opp_table>;
    
    gpu_kodi_opp_table: opp-table {
        compatible = "operating-points-v2";
        
        opp-200000000 {
            opp-hz = /bits/ 64 <200000000>;
            opp-microvolt = <650000>;
        };
        opp-400000000 {
            opp-hz = /bits/ 64 <400000000>;
            opp-microvolt = <700000>;
        };
        opp-600000000 {
            opp-hz = /bits/ 64 <600000000>;
            opp-microvolt = <750000>;
        };
    };
};

/* HDMI with CEC */
&hdmi {
    status = "okay";
    rockchip,hdmi21-support;
    rockchip,4k60-support;
    rockchip,hdr10-support;
};

&hdmi_cec {
    status = "okay";
    pinctrl-names = "default";
    pinctrl-0 = <&hdmi_cec_pins>;
};

/* Audio */
&i2s0_8ch {
    status = "okay";
    rockchip,sample-rate = <192000>;
    rockchip,channels = <8>;
    rockchip,bit-depth = <32>;
};

/* Network */
&gmac0 {
    status = "okay";
};

/* USB Storage */
&usbdrd_dwc3_0 {
    dr_mode = "host";
    status = "okay";
};

&usbdrd3_0 {
    status = "okay";
};

/* SD Card */
&sdmmc {
    status = "okay";
};

/* Common peripherals */
&uart2 {
    status = "okay";
};

&tsadc {
    status = "okay";
};

EOF
}

# Function to create GameScope-optimized DTS
create_gamescope_dts() {
    local distro=$1
    local kernel=$2
    
    cat << EOF
/dts-v1/;
#include <dt-bindings/gpio/gpio.h>
#include <dt-bindings/pinctrl/rockchip.h>
#include "rk3588s.dtsi"
#include "rk3588s-orangepi-5.dtsi"

/ {
    model = "Orange Pi 5 Plus - ${distro} ${kernel} - GameScope Gaming";
    compatible = "xunlong,orangepi-5-plus", "rockchip,rk3588s";
    
    metadata {
        build-type = "gamescope-pi";
        kernel-version = "${kernel}";
        distro-name = "$(echo ${distro} | cut -d'-' -f1)";
        distro-version = "$(echo ${distro} | cut -d'-' -f2)";
        gpu-driver = "valhall-g610";
        features = "gaming-optimized,low-latency,freesync";
    };

    chosen {
        bootargs = "console=ttyS2,1500000n8 root=/dev/mmcblk0p2 rootwait rw quiet splash mitigations=off threadirqs";
    };

    gamescope {
        compatible = "valve,gamescope";
        default-mode = "720p";
        target-fps = <60>;
        steam-deck-mode;
        
        display {
            adaptive-sync;
            hdr-enabled;
            color-gamut = "srgb";
        };
        
        performance {
            compositor-priority = "realtime";
            gpu-priority = "high";
            low-latency-mode;
        };
    };

    reserved-memory {
        #address-cells = <2>;
        #size-cells = <2>;
        ranges;

        linux,cma {
            compatible = "shared-dma-pool";
            reusable;
            size = <0x0 0x80000000>; /* 2GB for gaming */
            alignment = <0x0 0x400000>;
            linux,cma-default;
        };
    };
};

/* High-performance CPU */
&cluster0_opp_table {
    opp-2016000000 {
        opp-hz = /bits/ 64 <2016000000>;
        opp-microvolt = <1000000>;
        clock-latency-ns = <40000>;
    };
};

&cluster1_opp_table {
    opp-2400000000 {
        opp-hz = /bits/ 64 <2400000000>;
        opp-microvolt = <1050000>;
        clock-latency-ns = <40000>;
    };
    
    opp-2600000000 {
        opp-hz = /bits/ 64 <2600000000>;
        opp-microvolt = <1100000>;
        clock-latency-ns = <40000>;
        turbo-mode;
    };
};

/* Gaming GPU configuration */
&gpu {
    status = "okay";
    mali,scheduler = "performance";
    mali,power-policy = "always-on";
    mali,core-mask = <0xf>;
    operating-points-v2 = <&gpu_gaming_opp_table>;
    
    gpu_gaming_opp_table: opp-table {
        compatible = "operating-points-v2";
        
        opp-500000000 {
            opp-hz = /bits/ 64 <500000000>;
            opp-microvolt = <700000>;
        };
        opp-800000000 {
            opp-hz = /bits/ 64 <800000000>;
            opp-microvolt = <850000>;
        };
        opp-1000000000 {
            opp-hz = /bits/ 64 <1000000000>;
            opp-microvolt = <950000>;
        };
        opp-1200000000 {
            opp-hz = /bits/ 64 <1200000000>;
            opp-microvolt = <1050000>;
            turbo-mode;
        };
    };
};

/* Gaming display */
&hdmi {
    status = "okay";
    rockchip,hdmi21-frl-support;
    rockchip,4k120-support;
    rockchip,vrr-support;
    rockchip,allm-support;
    rockchip,qft-support;
};

&display_subsystem {
    status = "okay";
    rockchip,prefer-async-commit;
    rockchip,low-latency-mode;
};

/* PCIe for GPU/NVMe */
&pcie3x4 {
    status = "okay";
    max-link-speed = <3>;
    num-lanes = <4>;
};

/* USB for gaming peripherals */
&usbdrd_dwc3_0 {
    dr_mode = "host";
    maximum-speed = "super-speed";
    status = "okay";
};

&usbdrd3_0 {
    status = "okay";
};

/* Low latency audio */
&i2s0_8ch {
    status = "okay";
    rockchip,sample-rate = <48000>;
    rockchip,channels = <2>;
    rockchip,bit-depth = <24>;
    rockchip,low-latency;
};

/* Network for online gaming */
&gmac0 {
    status = "okay";
    snps,txpbl = <0x4>;
    snps,rxpbl = <0x4>;
    snps,aal;
    snps,tso;
};

/* Fan control */
&pwm1 {
    status = "okay";
};

/ {
    fan0: pwm-fan {
        compatible = "pwm-fan";
        pwms = <&pwm1 0 50000 0>;
        #cooling-cells = <2>;
        cooling-levels = <0 100 150 200 255>;
    };
};

/* Common peripherals */
&uart2 {
    status = "okay";
};

&tsadc {
    status = "okay";
};

EOF
}

# Generate Kodi-optimized DTS files
echo "Generating Kodi-optimized DTS files..."
for distro in "${DISTROS[@]}"; do
    for kernel in "${KERNELS[@]}"; do
        filename="rk3588s-orangepi-5-plus-${distro}-${kernel}-kodi.dts"
        echo "Creating ${filename}..."
        create_kodi_dts "${distro}" "${kernel}" > "${DEVICETREE_DIR}/${filename}"
    done
done

# Generate GameScope-optimized DTS files
echo "Generating GameScope-optimized DTS files..."
for distro in "${DISTROS[@]}"; do
    for kernel in "${KERNELS[@]}"; do
        filename="rk3588s-orangepi-5-plus-${distro}-${kernel}-gamescope.dts"
        echo "Creating ${filename}..."
        create_gamescope_dts "${distro}" "${kernel}" > "${DEVICETREE_DIR}/${filename}"
    done
done

echo "Special DTS file generation complete!"
echo "Generated $(ls -1 ${DEVICETREE_DIR}/*-kodi.dts ${DEVICETREE_DIR}/*-gamescope.dts 2>/dev/null | wc -l) special DTS files"