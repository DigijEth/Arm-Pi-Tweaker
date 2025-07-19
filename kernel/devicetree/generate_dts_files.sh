#!/bin/bash

# Generate DTS files for Orange Pi 5 Plus configurations
# This script creates DTS files for all supported distributions, kernels, and GPU drivers

DEVICETREE_DIR="."
DISTROS=("debian-11" "debian-12" "debian-13" "ubuntu-22.04" "ubuntu-24.04" "ubuntu-25.04")
KERNELS=("6.8" "6.9" "6.10" "6.11" "6.12" "6.13" "6.14" "6.15.4")
GPU_DRIVERS=("valhall-g610" "bifrost" "panfrost" "mesa")

# Function to create base DTS content
create_base_dts() {
    local distro=$1
    local kernel=$2
    local gpu_driver=$3
    local build_type=$4
    
    cat << EOF
/dts-v1/;
#include <dt-bindings/gpio/gpio.h>
#include <dt-bindings/pinctrl/rockchip.h>
#include "rk3588s.dtsi"
#include "rk3588s-orangepi-5.dtsi"

/ {
    model = "Orange Pi 5 Plus - ${distro} - Kernel ${kernel} - ${gpu_driver}";
    compatible = "xunlong,orangepi-5-plus", "rockchip,rk3588s";
    
    metadata {
        build-type = "${build_type}";
        kernel-version = "${kernel}";
        distro-name = "$(echo ${distro} | cut -d'-' -f1)";
        distro-version = "$(echo ${distro} | cut -d'-' -f2)";
        gpu-driver = "${gpu_driver}";
    };

    chosen {
        bootargs = "console=ttyS2,1500000n8 root=/dev/mmcblk0p2 rootwait rw";
    };
};

EOF
}

# Function to add Valhall G610 GPU configuration
add_valhall_gpu() {
    cat << 'EOF'
/* Mali G610 Valhall GPU configuration */
&gpu {
    compatible = "arm,mali-g610", "arm,mali-valhall";
    mali-supply = <&vdd_gpu_s0>;
    operating-points-v2 = <&gpu_opp_table>;
    status = "okay";
    
    gpu_opp_table: opp-table {
        compatible = "operating-points-v2";
        
        opp-300000000 {
            opp-hz = /bits/ 64 <300000000>;
            opp-microvolt = <675000>;
        };
        opp-400000000 {
            opp-hz = /bits/ 64 <400000000>;
            opp-microvolt = <675000>;
        };
        opp-600000000 {
            opp-hz = /bits/ 64 <600000000>;
            opp-microvolt = <700000>;
        };
        opp-800000000 {
            opp-hz = /bits/ 64 <800000000>;
            opp-microvolt = <800000>;
        };
        opp-1000000000 {
            opp-hz = /bits/ 64 <1000000000>;
            opp-microvolt = <900000>;
        };
    };
};

EOF
}

# Function to add Bifrost GPU configuration
add_bifrost_gpu() {
    cat << 'EOF'
/* Mali Bifrost GPU configuration (compatibility mode) */
&gpu {
    compatible = "arm,mali-bifrost", "arm,mali-midgard";
    mali-supply = <&vdd_gpu_s0>;
    operating-points-v2 = <&gpu_opp_table>;
    status = "okay";
    
    gpu_opp_table: opp-table {
        compatible = "operating-points-v2";
        
        opp-200000000 {
            opp-hz = /bits/ 64 <200000000>;
            opp-microvolt = <650000>;
        };
        opp-300000000 {
            opp-hz = /bits/ 64 <300000000>;
            opp-microvolt = <650000>;
        };
        opp-400000000 {
            opp-hz = /bits/ 64 <400000000>;
            opp-microvolt = <700000>;
        };
        opp-600000000 {
            opp-hz = /bits/ 64 <600000000>;
            opp-microvolt = <800000>;
        };
        opp-800000000 {
            opp-hz = /bits/ 64 <800000000>;
            opp-microvolt = <900000>;
        };
    };
};

EOF
}

# Function to add Panfrost/Mesa GPU configuration
add_panfrost_gpu() {
    cat << 'EOF'
/* Panfrost open-source GPU driver configuration */
&gpu {
    compatible = "rockchip,rk3588-mali", "arm,mali-g610";
    mali-supply = <&vdd_gpu_s0>;
    operating-points-v2 = <&gpu_opp_table>;
    status = "okay";
    
    /* Panfrost specific properties */
    #cooling-cells = <2>;
    dynamic-power-coefficient = <2982>;
    
    gpu_opp_table: opp-table {
        compatible = "operating-points-v2";
        
        opp-200000000 {
            opp-hz = /bits/ 64 <200000000>;
            opp-microvolt = <675000>;
        };
        opp-300000000 {
            opp-hz = /bits/ 64 <300000000>;
            opp-microvolt = <675000>;
        };
        opp-500000000 {
            opp-hz = /bits/ 64 <500000000>;
            opp-microvolt = <750000>;
        };
        opp-800000000 {
            opp-hz = /bits/ 64 <800000000>;
            opp-microvolt = <850000>;
        };
        opp-1000000000 {
            opp-hz = /bits/ 64 <1000000000>;
            opp-microvolt = <950000>;
        };
    };
};

EOF
}

# Function to add common peripherals
add_common_peripherals() {
    cat << 'EOF'
/* Enable common peripherals */
&gmac0 {
    status = "okay";
};

&i2c0 {
    status = "okay";
};

&i2c2 {
    status = "okay";
};

&i2c6 {
    status = "okay";
};

&pcie2x1l2 {
    status = "okay";
};

&pcie3x4 {
    status = "okay";
};

&sdhci {
    status = "okay";
};

&sdmmc {
    status = "okay";
};

&tsadc {
    status = "okay";
};

&uart2 {
    status = "okay";
};

&usb_host0_ehci {
    status = "okay";
};

&usb_host0_ohci {
    status = "okay";
};

&usb_host1_ehci {
    status = "okay";
};

&usb_host1_ohci {
    status = "okay";
};

&usb_host2_xhci {
    status = "okay";
};

EOF
}

# Generate standard desktop/server DTS files
echo "Generating standard desktop/server DTS files..."
for distro in "${DISTROS[@]}"; do
    for kernel in "${KERNELS[@]}"; do
        for gpu in "${GPU_DRIVERS[@]}"; do
            filename="rk3588s-orangepi-5-plus-${distro}-${kernel}-${gpu}-desktop.dts"
            echo "Creating ${filename}..."
            
            {
                create_base_dts "${distro}" "${kernel}" "${gpu}" "desktop"
                
                case "${gpu}" in
                    "valhall-g610")
                        add_valhall_gpu
                        ;;
                    "bifrost")
                        add_bifrost_gpu
                        ;;
                    "panfrost"|"mesa")
                        add_panfrost_gpu
                        ;;
                esac
                
                add_common_peripherals
            } > "${DEVICETREE_DIR}/${filename}"
        done
    done
done

echo "DTS file generation complete!"
echo "Generated $(ls -1 ${DEVICETREE_DIR}/*.dts | wc -l) DTS files"