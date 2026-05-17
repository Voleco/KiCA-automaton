# KiCA-automaton

[ 🇨🇳 点击这里跳转至中文版 (Jump to Chinese version) ](#-中文版)

KiCA (**Kizuna-Causality Automaton**) is a cellular automaton designed to simulate and evolve the universe. This repository provides a high-performance computing and visualization platform for it.

The project utilizes C++ as the underlying high-performance evolution engine. For the MVP (Minimum Viable Product) phase, it features a Python-based graphical user interface (via PySide6), with plans to potentially transition to a Web-based GUI in the future.

## 💡 Core Mathematical Paradigm & Intuition

Any grand unified theory aiming to reconcile general relativity and quantum mechanics inevitably requires a fundamental shift in mathematical paradigms. 

**At its lowest level, the core paradigm of KiCA steps away from traditional metric spaces and returns to fundamental sets and graphs. In this framework, the formulation of limits transitions from coordinate compactness to the compactness of local relations. This shift systematically eliminates the fundamental infinity/divergence issues inherently caused by the traditional modeling of "infinitesimal points + continuous fields."**

If you have previously pondered these foundational questions, this physical insight will allow you to quickly assess the plausibility of this framework within minutes, helping you decide whether to dive into the rigorous mathematical derivations in our papers.

Below are my popular science articles and intuitive explanations published on Zhihu (currently in Chinese):

- 🔗 **[[Zhihu: Taming Infinities: From Smooth Manifolds to Discrete Graphs](https://zhuanlan.zhihu.com/p/2038637024895296424)]**
- 🔗 **[[Zhihu: The KiCA Graph Encoding-Manifold Correspondence Conjecture](https://zhuanlan.zhihu.com/p/2038955685602387181)]**

## 📄 Related Papers

The theoretical foundation and core algorithms of this project are detailed in our research papers:

> **[Emergence of Physics from the Kizuna-Causality Automaton]**
> 
> Introduces the automaton and explains how it is compatible with and expands upon existing quantum mechanics frameworks.
> 
> 🔗 **[Click here to read the PDF](./docs/2026_KiCA.pdf)**

> **[A Universal Formula for Gravitational Effects Across All Scales Derived from Emergent Gravity in KiCA]**
> 
> Explains how Rule 2 of the automaton's evolution rules automatically generates gravitational effects, and provides a quantitative formula for gravity that is compatible with and expands upon the existing framework of relativity.
> 
> 🔗 **[Click here to read the PDF](./docs/2026_Gravity_in_KiCA.pdf)**

*(Note: Please view the full PDF documents in the `docs/` directory after cloning this repository.)*

## 🏗️ Architecture

This project adopts a Monorepo design based on the "Onion Architecture", achieving complete decoupling of the core algorithms from the presentation layer:

- `core_engine/`: A high-performance evolution engine written purely in C++ (with no UI or external dependencies).
- `interfaces/`: The bridge between the engine and the outside world (currently includes `pybind11` Python bindings, with future plans for a WebSocket Server).
- `frontends/`: The user interaction interface (currently an MVP desktop application based on `PySide6`).
- `docs/`: Papers, supplementary materials, and documentation.

## 🚀 Quick Start (MVP Phase)

### 1. Prerequisites
- **C++ Compiler**: CMake (>= 3.15), GCC/Clang/MSVC (C++17 support required)
- **Python Environment**: Python 3.10+
- **Package Manager**: [uv](https://github.com/astral-sh/uv) (Recommended for lightning-fast dependency management)

### 2. Install Dependencies
Navigate to the Python frontend directory and use `uv` to sync dependencies:
```bash
cd frontends/python_gui
uv sync
```

### 3. Build the Core Engine
*(TODO: CMake build commands for compiling the C++ engine into a Python extension module.)*

---

<br>

# 🇨🇳 中文版

KiCA (**Kizuna-Causality Automaton**) 本身是一个用于模拟和演化宇宙的元胞自动机。本项目旨在为其提供一个高性能计算与可视化平台。

本项目采用 C++ 作为底层高性能演化引擎，并计划在 MVP 阶段使用 Python (PySide6) 构建图形化控制界面，后续视情况可能转换为基于 Web 的图形界面。

## 💡 核心数学范式与设计直觉

任何旨在统一相对论与量子力学的大一统理论，必然伴随着数学范式（Mathematical Paradigm）上的根本性转变。

**在最底层的数学方法论上，KiCA 的核心范式脱离了传统的度量空间，回归到最基础的集合与图。在这一框架下，极限的表述从“坐标的紧致性”转变为“局部关系的紧致性”。这种视角的转变，系统性地消除了传统“无穷小点 + 连续场”建模所固有的无穷大/发散问题。**

如果你曾深入思考过基础物理的相关问题，上述的物理直觉将帮助你在短短几分钟内，快速判断这一框架的可行性与合理性，从而决定是否需要投入时间去阅读论文中的严密推导。

以下是我在知乎上发布的系列科普与解析文章：


- 🔗 **[[知乎专栏：驯服无穷大：从光滑流形来到离散图](https://zhuanlan.zhihu.com/p/2038637024895296424)]**
- 🔗 **[[知乎专栏：KiCA 图编码与流形对应猜想](https://zhuanlan.zhihu.com/p/2038955685602387181)]**



## 📄 相关论文 (Paper)

本项目的理论基础与核心算法详见我们的研究论文：

> **[Emergence of Physics from the Kizuna-Causality Automaton]**
> 
> 介绍该自动机，以及为什么其能兼容、拓展现有量子力学框架。
> 
> 🔗 **[点击此处阅读论文 PDF](./docs/2026_KiCA.pdf)**

> **[A Universal Formula for Gravitational Effects Across All Scales Derived from Emergent Gravity in KiCA]**
> 
> 探讨该自动机演化规则中的 Rule 2 如何自动产生引力效应，并给出一个兼容且拓展现有相对论框架的引力定量计算公式。
> 
> 🔗 **[点击此处阅读论文 PDF](./docs/2026_Gravity_in_KiCA.pdf)**

*(注：请在克隆本仓库后，于 `docs/` 目录下查看完整的 PDF 文档。)*

## 🏗️ 工程架构 (Architecture)

本项目采用“洋葱架构 (Onion Architecture)”的 Monorepo 设计，实现了核心算法与表现层的完全解耦：

- `core_engine/`: 纯 C++ 编写的高性能演化引擎（无任何 UI 或外部依赖）。
- `interfaces/`: 引擎与外部世界的桥梁（当前包含 `pybind11` 的 Python 绑定，未来扩展 WebSocket Server）。
- `frontends/`: 用户交互界面（当前为基于 `PySide6` 的 MVP 桌面端）。
- `docs/`: 论文、补充材料与文档。

## 🚀 快速开始 (Quick Start - MVP Phase)

### 1. 环境依赖
- **C++ 编译环境**: CMake (>= 3.15), GCC/Clang/MSVC (支持 C++17)
- **Python 环境**: Python 3.10+
- **包管理工具**: 推荐使用现代化的 [uv](https://github.com/astral-sh/uv)

### 2. 安装依赖
进入 Python 前端目录，使用 `uv` 极速安装并同步依赖：
```bash
cd frontends/python_gui
uv sync
```
*(如果没有安装 uv，也可以使用传统的 `pip install pybind11 PySide6 pyqtgraph`)*

### 3. 编译核心引擎
*(待补充：使用 CMake 编译 C++ 引擎并生成 Python 扩展模块的具体指令)*
