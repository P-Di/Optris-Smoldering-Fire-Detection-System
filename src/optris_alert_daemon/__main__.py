from __future__ import annotations

import argparse
import logging

from .config import ConfigError, load_config
from .monitor import MonitorService
from .optris_sdk import OptrisSdkError


def main() -> int:
    parser = argparse.ArgumentParser(description="Monitor Optris PIX Connect alarms and send email alerts.")
    parser.add_argument("--config", required=True, help="Path to the JSON config file.")
    parser.add_argument(
        "--validate",
        action="store_true",
        help="Only validate the config file and print a summary.",
    )
    args = parser.parse_args()

    try:
        config = load_config(args.config)
        if args.validate:
            logging.basicConfig(level=logging.INFO, format="%(levelname)s: %(message)s")
            service = MonitorService(config)
            service.validate()
            return 0

        service = MonitorService(config)
        service.run()
        return 0
    except (ConfigError, OptrisSdkError, RuntimeError) as exc:
        logging.basicConfig(level=logging.ERROR, format="%(levelname)s: %(message)s")
        logging.error(str(exc))
        return 1
    except KeyboardInterrupt:
        return 130
